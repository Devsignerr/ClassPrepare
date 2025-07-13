#include "C_ActionComponent.h"
#include "C_BattleComponent.h"
#include "C_CrowdControlComponent.h"
#include "C_LockOnComponent.h"
#include "C_SkillComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectC/Data/C_PlayerDataAsset.h"
#include "..\..\Interface\C_CharacterInterface.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"
#include "ProjectC/Utils/C_GameUtil.h"

UC_ActionComponent::UC_ActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_ActionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
	IC_CharacterInterface* Interface = CastChecked<IC_CharacterInterface>(OwnerCharacter);

	if (!Interface->GetOnLandedDelegate()->IsBoundToObject(this))
		Interface->GetOnLandedDelegate()->AddUObject(this, &ThisClass::OnLanded);
}

void UC_ActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsInSpecialAction && !OwnerCharacter->GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
	{
		FRotator LookAtRot = OwnerCharacter->GetBaseAimRotation();
		LookAtRot.Pitch = 0.f; // 평면 회전만

		const FRotator CurrentRot = OwnerCharacter->GetActorRotation();
		const FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot, GetWorld()->GetDeltaSeconds(), 10.f);
			 	
		OwnerCharacter->SetActorRotation(NewRot);
	}
}

void UC_ActionComponent::Move(FVector2D MovementVector)
{
	InputVector = MovementVector;
	
	if (!CanAction(EC_ActionType::Move))
		return;

	const IC_PlayerCharacterInterface* PlayerInterface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	const IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
	const UC_LockOnComponent* LockOnComponent = PlayerInterface->GetLockOnComponent();
	check(LockOnComponent);

	UC_SkillComponent* SkillComponent = CharacterInterface->GetSkillComponent();
	check(SkillComponent);

	UC_BattleComponent* BattleComponent = CharacterInterface->GetBattleComponent();
	check(BattleComponent);
	
	if (!LockOnComponent->IsLockOnMode() || IsRunning)
	{
		ProcessFreeMove();
	}
	else
	{
		ProcessLockOnMove();
	}
		
	if (IsAttacking && BattleComponent->CharacterStanceType == EC_CharacterStanceType::Sword)
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

	OwnerCharacter->MakeNoise(1, OwnerCharacter.Get(), OwnerCharacter->GetActorLocation());
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PlayerData->JumpSound, OwnerCharacter->GetActorLocation());

	//AddLock(EC_LockCauseType::Jump, EC_ActionType::Attack);
}

void UC_ActionComponent::Attack(bool IsPressed)
{
	if (!IsPressed)
		return;

	if (!CanAction(EC_ActionType::Attack))
		return;

	const IC_PlayerCharacterInterface* PlayerInterface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	const IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = PlayerInterface->GetPlayerData();
	check(PlayerData);

	UC_BattleComponent* BattleComponent = CharacterInterface->GetBattleComponent();
	check(BattleComponent);

	TArray<UAnimMontage*> Montages = FC_GameUtil::GetComboAttackMontages(PlayerData, BattleComponent->CharacterStanceType, IsInSpecialAction);
	if (Montages.IsEmpty())
		return;
	
	if (IsAttacking)
	{
		SaveAttack = true;
	}
	else
	{
		IsAttacking = true;
		AttackCount++;
		
		if (AttackCount > Montages.Num())
		{
			AttackCount = 0;
		}
		
		if (Montages.IsValidIndex(AttackCount - 1))
		{
			OwnerCharacter->PlayAnimMontage(Montages[AttackCount - 1]);
		}

		RotateToControlRotation();

		if (EC_CharacterStanceType::Sword == BattleComponent->CharacterStanceType)
			AddLock(EC_LockCauseType::Attack, EC_ActionType::Move);
		
		AddLock(EC_LockCauseType::Attack, EC_ActionType::Jump);
		AddLock(EC_LockCauseType::Attack, EC_ActionType::SpecialAction);
	}
}


void UC_ActionComponent::OnGuardSuccess(AActor* DamageCauser)
{
	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	FVector DamageCauserLocation = DamageCauser->GetActorLocation();

	FVector ForceDir = (OwnerLocation - DamageCauserLocation).GetSafeNormal2D();
	OwnerCharacter->GetCharacterMovement()->Velocity += ForceDir * 800.f;
}

void UC_ActionComponent::OnLanded()
{
	//ForceReleaseLock(EC_LockCauseType::Jump);
}

void UC_ActionComponent::ComboAttackSave()
{
	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	const IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);

	UC_BattleComponent* BattleComponent = CharacterInterface->GetBattleComponent();
	check(BattleComponent);

	TArray<UAnimMontage*> Montages = FC_GameUtil::GetComboAttackMontages(PlayerData, BattleComponent->CharacterStanceType, IsInSpecialAction);
	
	if (SaveAttack)
	{
		SaveAttack = false;
		AttackCount++;
		
		if (AttackCount > Montages.Num())
		{
			AttackCount = 0;
		}

		if (Montages.IsValidIndex(AttackCount - 1))
			OwnerCharacter->PlayAnimMontage(Montages[AttackCount - 1]);

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

void UC_ActionComponent::SpecialAction(bool bPressed)
{
	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);

	IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
	UC_SkillComponent* SkillComponent = CharacterInterface->GetSkillComponent();
	check(SkillComponent);
	
	if (bPressed && !IsInSpecialAction)
	{
		if (!CanAction(EC_ActionType::SpecialAction))
			return;
		
		IsInSpecialAction = true;
		ResetCombo();

		AddLock(EC_LockCauseType::SpecialAction, EC_ActionType::Run);
	}
	else if (!bPressed && IsInSpecialAction)
	{
		IsInSpecialAction = false;

		//SkillComponent->RequestStopSkill()

		ForceReleaseLock(EC_LockCauseType::SpecialAction);
	}
}

void UC_ActionComponent::Run(bool bPressed)
{
	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
	check(PlayerData);
	
	if (bPressed && !IsRunning)
	{
		if (!CanAction(EC_ActionType::Run))
			return;
		
		IsRunning = true;

		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerData->MovementSpeed_Sprint;
	}
	else if (!bPressed && IsRunning)
	{
		IsRunning = false;
		
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerData->MovementSpeed_Jog;
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
		const IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
		
		UC_BattleComponent* BattleComponent = CharacterInterface->GetBattleComponent();
		check(BattleComponent);

		UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
		check(PlayerData);
		
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Move);
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Attack);
		AddLock(EC_LockCauseType::Roll, EC_ActionType::Jump);
		AddLock(EC_LockCauseType::Roll, EC_ActionType::SpecialAction);
		
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

	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
	check(CharacterInterface);

	UC_CrowdControlComponent* CrowdControlComponent = CharacterInterface->GetCrowdControlComponent();
	check(CrowdControlComponent);

	if (CrowdControlComponent->IsCrowdControlled())
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
