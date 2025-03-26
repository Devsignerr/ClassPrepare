// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClassCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/C_BattleComponent.h"
#include "Component/C_StatComponent.h"

AClassCharacter::AClassCharacter()
{
	// 캐릭터의 Capsule Component 사이즈
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// 컨트롤러 인풋이 카메라에만 영향을 끼치도록 함 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//  true 로 하면 입력에 따라 캐릭터가 회전함 . 
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f; // 아날로그 스틱 입력값

	// Braking Deceleration Walking	멈출 때 얼마나 빨리 감속할지 결정	기본값: 2048 → 줄이면 미끄러짐
	// Ground Friction	지면 마찰력. 작을수록 더 오래 미끄러짐	기본값: 8 → 줄이면 미끄러짐 강화
	// Braking Friction Factor	브레이킹 중 마찰력 계수. 0이면 브레이크 안 걸리는 느낌	기본값: 1 → 0~0.3 추천
	GetCharacterMovement()->BrakingDecelerationWalking = 500.f;
	GetCharacterMovement()->GroundFriction = 2.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;

	//컨트롤러의 입력에 따라 회전함
	CameraBoom->bUsePawnControlRotation = true; 
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	// 카메라가 어태치된 부모의 회전값을 따라감. true 로 하면 입력값에 따라 회전해버림 .
	// true 인 경우는 1인칭 게임일때
	FollowCamera->bUsePawnControlRotation = false;

	BattleComponent = CreateDefaultSubobject<UC_BattleComponent>(TEXT("BattleComponent"));
	StatComponent = CreateDefaultSubobject<UC_StatComponent>(TEXT("StatComponent"));
}

void AClassCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			
			//0: 기본 입력
			//1~n: 특정 상황에만 쓰는 입력 (UI 모드, 전투 모드, 미니게임 등)
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AClassCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatComponent->OnStatChanged.AddUObject(this, &ThisClass::ApplyStat);
}

void AClassCharacter::ApplyStat(const FC_CharacterStat& BaseStat, const FC_CharacterStat& ModifierStat)
{
	float MovementSpeed = (BaseStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input
//입력(Key, Axis, Input Action 등)을 실제 함수와 바인딩하는 함수. 언리얼이 자동 호출해줌
// 플레이어가 해당 Pawn 또는 Character를 Possess 했을 때 자동으로 이 함수가 호출
void AClassCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Triggered는 키가 눌리고 있을때 반복적으로 호출됨.
		//이게 싫다면 IA Usset 에서 언제 트리거 이벤트를 호출할지 결정해줘야 함 
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClassCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AClassCharacter::Look);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AClassCharacter::Attack);

	}
}

void AClassCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 이동방향을 결정하기 위해 컨트롤러의 회전값을 알아냄
		// FRotationMatrix는 회전(Rotation)을 3x3 방향 벡터 행렬로 바꿔주는 클래스
		// 이 회전값 기준으로 좌표축을 만들자는 의미
		// X축 방향 벡터를 뽑아냅니다.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		// float3x3 RotationMatrix(float3 vAxis, float fAngle) float 3x3 을 반환한다는것을 알수 있음
		// X의 경우 회전 행렬의 첫 번째 행을 벡터로 만들어서 반환하는 거고,
		// 즉, 현재 회전값에서의 X축 방향(= Forward Vector) 을 의미함
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AClassCharacter::Look(const FInputActionValue& Value)
{
	// 마우스 인풋을 2D 벡터로 저장
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		//컨트롤러 회전값 입력
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AClassCharacter::Attack(const FInputActionValue& Value)
{
	
}

void AClassCharacter::AttackTrace(bool bStart, FName TraceBoneName)
{
	if (BattleComponent)
	{
		if (bStart)
			BattleComponent->StartTrace(TraceBoneName);
		else
			BattleComponent->EndTrace();
	}
}




