#include "C_PlayableCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Component/C_ActionComponent.h"
#include "Component/C_LockOnComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "ProjectC/Data/C_InputDataAsset.h"
#include "ProjectC/UI/C_HUDWidget.h"

AC_PlayableCharacter::AC_PlayableCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;

	//컨트롤러의 입력에 따라 회전함
	CameraBoom->bUsePawnControlRotation = true; 
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	StimulusSource->RegisterForSense(TSubclassOf<UAISense>(UAISense_Sight::StaticClass()));
	StimulusSource->RegisterWithPerceptionSystem();

	LockOnComponent = CreateDefaultSubobject<UC_LockOnComponent>(TEXT("LockOnComponent"));
	ActionComponent = CreateDefaultSubobject<UC_ActionComponent>(TEXT("ActionComponent"));
	
	// 카메라가 어태치된 부모의 회전값을 따라감. true 로 하면 입력값에 따라 회전해버림 .
	// true 인 경우는 1인칭 게임일때
	FollowCamera->bUsePawnControlRotation = false;
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

void AC_PlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetGenericTeamId(FGenericTeamId(0));
}

void AC_PlayableCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InputData->JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(InputData->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(InputData->MoveAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Move);
		EnhancedInputComponent->BindAction(InputData->LookAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Look);
		EnhancedInputComponent->BindAction(InputData->GuardAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Guard);
		EnhancedInputComponent->BindAction(InputData->AttackAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Attack);
		EnhancedInputComponent->BindAction(InputData->LockOnAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::LockOn);
		EnhancedInputComponent->BindAction(InputData->RunAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Run);
		EnhancedInputComponent->BindAction(InputData->RollAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Roll);
	}
}

void AC_PlayableCharacter::Move(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();
	
	check(ActionComponent);
	ActionComponent->Move(Movement);
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

void AC_PlayableCharacter::Jump(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	
	check(ActionComponent);
	ActionComponent->Jump(IsPressed);
}

void AC_PlayableCharacter::Attack(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	
	check(ActionComponent);
	ActionComponent->Attack(IsPressed);
}

void AC_PlayableCharacter::Guard(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	
	check(ActionComponent);
	ActionComponent->Guard(IsPressed);
}

void AC_PlayableCharacter::Run(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	
	check(ActionComponent);
	ActionComponent->Run(IsPressed);
}

void AC_PlayableCharacter::Roll(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;

	check(ActionComponent);
	ActionComponent->Roll(IsPressed);
}

void AC_PlayableCharacter::LockOn(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	if (!IsPressed)
		return;

	check(LockOnComponent);
	LockOnComponent->LockOn();
}

void AC_PlayableCharacter::SetupHUDWidget(UC_HUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->UpdateStat(StatComponent->GetBaseStat(), StatComponent->GetModifierStat());
		StatComponent->OnStatChanged.AddUObject(InHUDWidget, &UC_HUDWidget::UpdateStat);
	}
}

void AC_PlayableCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	GenericTeamId = InTeamID;
}

FGenericTeamId AC_PlayableCharacter::GetGenericTeamId() const
{
	return GenericTeamId;
}


