#include "C_PlayableCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AC_PlayableCharacter::AC_PlayableCharacter()
{
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
}

void AC_PlayableCharacter::Attack(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	if (!IsPressed)
		return;

	if (IsAttacking)
	{
		SaveAttack = true;
	}
	else
	{
		IsAttacking = true;
		AttackCount++;
		
		if (AttackCount > AttackMontages.Num())
		{
			AttackCount = 0;
		}

		if (AttackMontages.IsValidIndex(AttackCount - 1))
			PlayAnimMontage(AttackMontages[AttackCount - 1]);
	}
}

void AC_PlayableCharacter::BeginPlay()
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

void AC_PlayableCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Triggered는 키가 눌리고 있을때 반복적으로 호출됨.
		//이게 싫다면 IA Usset 에서 언제 트리거 이벤트를 호출할지 결정해줘야 함 
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Look);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Attack);

	}
}

void AC_PlayableCharacter::ComboAttackSave()
{
	if (SaveAttack)
	{
		SaveAttack = false;
		AttackCount++;
		
		if (AttackCount > AttackMontages.Num())
		{
			AttackCount = 0;
		}

		if (AttackMontages.IsValidIndex(AttackCount - 1))
			PlayAnimMontage(AttackMontages[AttackCount - 1]);
	}
}

void AC_PlayableCharacter::ResetCombo()
{
	AttackCount = 0;
	SaveAttack = false;
	IsAttacking = false;
}

void AC_PlayableCharacter::Move(const FInputActionValue& Value)
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

void AC_PlayableCharacter::Look(const FInputActionValue& Value)
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

