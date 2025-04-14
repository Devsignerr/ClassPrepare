#include "C_ActionComponent.h"
#include "C_BattleComponent.h"
#include "C_LockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectC/Character/C_PlayableCharacter.h"
#include "ProjectC/Data/C_PlayerDataAsset.h"

UC_ActionComponent::UC_ActionComponent()
{
}

void UC_ActionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
}

void UC_ActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_ActionComponent::Move(FVector2D MovementVector)
{
	if (!CanAction(EC_ActionType::Move))
		return;

	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	const UC_LockOnComponent* LockOnComponent = Interface->GetLockOnComponent();
	check(LockOnComponent);
	
	InputVector = MovementVector;
	
	if (!LockOnComponent->IsLockOnMode() || IsRunning)
	{
		ProcessFreeMove();
	}
	else
	{
		ProcessLockOnMove();
	}
		
	if (IsAttacking)
	{
		if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.1f);
			ResetCombo();
		}
	}
}

void UC_ActionComponent::ProcessFreeMove()
{
	const APlayerController* PlayerController = CastChecked<APlayerController>(OwnerCharacter->GetController());
	
	const FRotator Rotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	OwnerCharacter->AddMovementInput(ForwardDirection, InputVector.Y);
	OwnerCharacter->AddMovementInput(RightDirection, InputVector.X);
}

void UC_ActionComponent::ProcessLockOnMove()
{
	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	const UC_LockOnComponent* LockOnComponent = Interface->GetLockOnComponent();
	check(LockOnComponent);
	
	if (AActor* TargetActor = LockOnComponent->GetLockTarget())
	{
		const FVector MyLocation = OwnerCharacter->GetActorLocation();
		const FVector TargetLocation = TargetActor->GetActorLocation();

		const FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();

		// 2. 궤도 이동 방향 계산
		const FVector OrbitRight = FVector::CrossProduct(FVector::UpVector, ToTarget); // 좌우
		const FVector OrbitForward = ToTarget;                                         // 앞뒤

		// 3. 최종 이동 방향
		FVector MoveDir = OrbitRight * InputVector.X + OrbitForward * InputVector.Y;
		MoveDir.Normalize();

		// 4. 이동
		OwnerCharacter->AddMovementInput(MoveDir);

		// 5. 타겟 바라보기
		FRotator LookAtRot = (TargetLocation - MyLocation).Rotation();
		LookAtRot.Pitch = 0.f; // 평면 회전만

		const FRotator CurrentRot = OwnerCharacter->GetActorRotation();
		const FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot, GetWorld()->GetDeltaSeconds(), 10.f);
			 	
		OwnerCharacter->SetActorRotation(NewRot);
	}
}

void UC_ActionComponent::Jump(bool IsPressed)
{
	if (!IsPressed)
		return;
	
	if (!CanAction(EC_ActionType::Jump))
		return;

	OwnerCharacter->Jump();

	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	const UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);

	OwnerCharacter->MakeNoise(1, OwnerCharacter, OwnerCharacter->GetActorLocation());
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PlayerData->JumpSound, OwnerCharacter->GetActorLocation());
}

void UC_ActionComponent::Attack(bool IsPressed)
{
	if (!IsPressed)
		return;

	if (!CanAction(EC_ActionType::Attack))
		return;

	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);

	TArray<UAnimMontage*>& AttackMontages = PlayerData->AttackMontages;

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
		{
			OwnerCharacter->PlayAnimMontage(AttackMontages[AttackCount - 1]);
		}

		RotateToControlRotation();

		AddLock(EC_LockCauseType::Attack, EC_ActionType::Move);
		AddLock(EC_LockCauseType::Attack, EC_ActionType::Jump);
	}
}

void UC_ActionComponent::ComboAttackSave()
{
	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);

	TArray<UAnimMontage*>& AttackMontages = PlayerData->AttackMontages;
	
	if (SaveAttack)
	{
		SaveAttack = false;
		AttackCount++;
		
		if (AttackCount > AttackMontages.Num())
		{
			AttackCount = 0;
		}

		if (AttackMontages.IsValidIndex(AttackCount - 1))
			OwnerCharacter->PlayAnimMontage(AttackMontages[AttackCount - 1]);

		RotateToControlRotation();
	}
	else
	{
		ForceReleaseLock(EC_LockCauseType::Attack);
	}
}

void UC_ActionComponent::ResetCombo()
{
	AttackCount = 0;
	SaveAttack = false;
	IsAttacking = false;
	ForceReleaseLock(EC_LockCauseType::Attack);
}

void UC_ActionComponent::RotateToControlRotation()
{
	const APlayerController* PlayerController = CastChecked<APlayerController>(OwnerCharacter->GetController());
	FRotator ControlRotation = PlayerController->GetControlRotation();
	ControlRotation.Pitch = 0.f;

	OwnerCharacter->SetActorRotation(ControlRotation);
}

void UC_ActionComponent::Guard(bool bPressed)
{
	if (bPressed && !IsGuarding)
	{
		if (!CanAction(EC_ActionType::Guard))
			return;
		
		IsGuarding = true;
	}
	else if (!bPressed && IsGuarding)
	{
		IsGuarding = false;
	}
}

void UC_ActionComponent::Run(bool bPressed)
{
	if (bPressed && !IsRunning)
	{
		if (!CanAction(EC_ActionType::Run))
			return;
		
		IsRunning = true;

		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 800.f;
	}
	else if (!bPressed && IsRunning)
	{
		IsRunning = false;
		
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

void UC_ActionComponent::Roll(bool bPressed)
{
	if (bPressed && !IsRolling)
	{
		if (!CanAction(EC_ActionType::Roll))
			return;
		
		const APlayerController* PlayerController = CastChecked<APlayerController>(OwnerCharacter->GetController());
		const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
		UC_BattleComponent* BattleComponent = Interface->GetBattleComponent();
		check(BattleComponent);

		UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
		check(PlayerData);
		
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Move);
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Attack);
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Jump);
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Guard);
		
		IsRolling = true;

		const FRotator Rotation = PlayerController->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		FVector RollDir = FVector::ZeroVector;
		RollDir += ForwardDirection * InputVector.Y;
		RollDir += RightDirection * InputVector.X;

		OwnerCharacter->SetActorRotation(RollDir.Rotation());
		
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		check(AnimInstance);
		
		AnimInstance->StopAllMontages(0.1f);
		OwnerCharacter->PlayAnimMontage(PlayerData->RollMontage);
		FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnMontageEnd);
		AnimInstance->Montage_SetEndDelegate(EndDelegate);
		
		BattleComponent->EndTrace();
	}
}

bool UC_ActionComponent::CanAction(EC_ActionType InActionType)
{
	if (!IsLocked(InActionType))
		return false;

	return true;
}

bool UC_ActionComponent::IsLocked(EC_ActionType InLockType)
{
	for (const auto& Lock : LockData)
	{
		if (Lock.LockType == InLockType)
			return false;
	}
	
	return true;
}

void UC_ActionComponent::AddLock(EC_LockCauseType InLockCauseType, EC_ActionType InLockType)
{
	FC_LockData Data(InLockCauseType, InLockType);
	LockData.Add(Data);
}

void UC_ActionComponent::ReleaseLock(EC_LockCauseType InLockCauseType, EC_ActionType InLockType)
{
	const int32 Index = LockData.IndexOfByPredicate([InLockCauseType, InLockType](const FC_LockData& Data)
	{
		return Data.LockType == InLockType && Data.LockCauseType == InLockCauseType;
	});
	
	if (Index != INDEX_NONE)
	{
		LockData.RemoveAt(Index);
	}
}

void UC_ActionComponent::ForceReleaseLock(EC_LockCauseType InLockCauseType)
{
	LockData.RemoveAll([InLockCauseType](const FC_LockData& Data)
	{
		return Data.LockCauseType == InLockCauseType;
	});
}

void UC_ActionComponent::ResetAllLock()
{
	LockData.Empty();
}

void UC_ActionComponent::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);
	
	if (Montage == PlayerData->RollMontage)
	{
		IsRolling = false;
		ForceReleaseLock(EC_LockCauseType::Roll);
		ForceReleaseLock(EC_LockCauseType::Attack);
	}
}

void UC_ActionComponent::OnGuardSuccess(AActor* DamageCauser)
{
	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	FVector DamageCauserLocation = DamageCauser->GetActorLocation();

	FVector ForceDir = (OwnerLocation - DamageCauserLocation).GetSafeNormal2D();
	OwnerCharacter->GetCharacterMovement()->AddForce(ForceDir * 100.f);
}
