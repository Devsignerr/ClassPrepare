#include "C_PlayableCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Component/C_LockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
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
	
	// 카메라가 어태치된 부모의 회전값을 따라감. true 로 하면 입력값에 따라 회전해버림 .
	// true 인 경우는 1인칭 게임일때
	FollowCamera->bUsePawnControlRotation = false;
}

void AC_PlayableCharacter::Attack(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	if (!IsPressed)
		return;

	if (IsRolling)
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

		bCanMove = false;
	}
}

void AC_PlayableCharacter::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == RollMontage)
	{
		IsRolling = false;
		bCanMove = true;
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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Look);

		//Guard Action
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Guard);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Attack);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::LockOnMode);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Run);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AC_PlayableCharacter::Roll);
	}
}

void AC_PlayableCharacter::SetupHUDWidget(UC_HUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->UpdateStat(StatComponent->GetBaseStat(), StatComponent->GetModifierStat());
		StatComponent->OnStatChanged.AddUObject(InHUDWidget, &UC_HUDWidget::UpdateStat);
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
	else
	{
		bCanMove = true;
	}
}

void AC_PlayableCharacter::ResetCombo()
{
	AttackCount = 0;
	SaveAttack = false;
	IsAttacking = false;
	bCanMove = true;
}

void AC_PlayableCharacter::LockOnMode(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;
	if (!IsPressed)
		return;

	SetLockOnMode(!IsLockOnMode());
}

void AC_PlayableCharacter::SetLockOnMode(bool bEnable)
{
	if (bEnable)
	{
		if (APawn* Target = LockOnComponent->FindTarget())
		{
			LockOnComponent->LockTarget(Target);
		}
		else
		{
			bEnable = false;
		}
	}

	if (!bEnable)
	{
		LockOnComponent->ClearTarget();
	}

	Super::SetLockOnMode(bEnable);
}

void AC_PlayableCharacter::Guard(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;

	if (IsPressed && !IsGuarding)
	{
		IsGuarding = true;
	}
	else if (!IsPressed && IsGuarding)
	{
		IsGuarding = false;
	}
}

void AC_PlayableCharacter::Run(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;

	if (IsPressed && !IsRunning)
	{
		IsRunning = true;

		GetCharacterMovement()->MaxWalkSpeed = 800.f;
		
	}
	else if (!IsPressed && IsRunning)
	{
		IsRunning = false;
		
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void AC_PlayableCharacter::Roll(const FInputActionValue& Value)
{
	const bool IsPressed = Value[0] != 0.f;

	if (IsPressed && !IsRolling)
	{
		bCanMove = false;
		IsRolling = true;

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement

		FVector RollDir = FVector::ZeroVector;
		RollDir += ForwardDirection * InputVector.Y;
		RollDir += RightDirection * InputVector.X;

		SetActorRotation(RollDir.Rotation());
		
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.1f);
			
			PlayAnimMontage(RollMontage);
			FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnMontageEnd);
			AnimInstance->Montage_SetEndDelegate(EndDelegate);
		}

		BattleComponent->EndTrace();
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

void AC_PlayableCharacter::Move(const FInputActionValue& Value)
{
	if (!bCanMove)
		return;
	
	if (Controller != nullptr)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		InputVector = MovementVector;
		
		if (!bLockOnMode || IsRunning)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
		else
		{
			 if (AActor* TargetActor = LockOnComponent->Target)
			 {
			 	const FVector MyLocation = GetActorLocation();
			 	const FVector TargetLocation = TargetActor->GetActorLocation();

			 	const FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();

			 	// 2. 궤도 이동 방향 계산
			 	const FVector OrbitRight = FVector::CrossProduct(FVector::UpVector, ToTarget); // 좌우
			 	const FVector OrbitForward = ToTarget;                                         // 앞뒤

			 	// 3. 최종 이동 방향
			 	FVector MoveDir = OrbitRight * MovementVector.X + OrbitForward * MovementVector.Y;
			 	MoveDir.Normalize();

			 	// 4. 이동
			 	AddMovementInput(MoveDir);

			 	// 5. 타겟 바라보기
			 	FRotator LookAtRot = (TargetLocation - MyLocation).Rotation();
			 	LookAtRot.Pitch = 0.f; // 평면 회전만

			 	const FRotator CurrentRot = GetActorRotation();
			 	const FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot, GetWorld()->GetDeltaSeconds(), 10.f);
			 	
			 	SetActorRotation(NewRot);
			 }
		}
		
		if (IsAttacking)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->StopAllMontages(0.1f);
				ResetCombo();
			}
		}
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

void AC_PlayableCharacter::Jump()
{
	if (!bCanMove)
		return;
	
	Super::Jump();

	MakeNoise(1, this, GetActorLocation());
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), JumpSound, GetActorLocation());
}

void AC_PlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetGenericTeamId(FGenericTeamId(0));
}

