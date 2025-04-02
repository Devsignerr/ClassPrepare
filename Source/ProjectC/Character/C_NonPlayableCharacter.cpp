
#include "C_NonPlayableCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Utils/C_GameUtil.h"

AC_NonPlayableCharacter::AC_NonPlayableCharacter()
{
}

void AC_NonPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FC_EnemyTableRow* TableRow = FC_GameUtil::GetEnemyData(CharacterType))
	{
		EnemyTableRow = TableRow;
	}
	check(EnemyTableRow);
	
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetSkeletalMesh(EnemyTableRow->SkeletalMesh);
		MeshComponent->SetAnimClass(EnemyTableRow->AnimInstance);
	}

	GetCharacterMovement()->MaxWalkSpeed = EnemyTableRow->MovementSpeed;
}

void AC_NonPlayableCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsTurning)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			const float CurveValue = AnimInstance->GetCurveValue(TEXT("DistanceToPivot"));
			UE_LOG(LogProjectC, Log, TEXT("%.2f"), CurveValue);
			const float MaxCurveVal = -FMath::Abs(TurnDegree);
			
			// 3. 회전 진행률 (비율)
			float TurnAlpha = 1.f - FMath::Clamp(CurveValue / MaxCurveVal, 0.f, 1.f);  // 0.0 ~ 1.0
			
			// 5. 회전 적용
			float FinalYaw = TurnStartYaw + (TurnDegree * TurnAlpha);
			FRotator NewRot = FRotator(0.f, FinalYaw, 0.f);
			NewRot = FMath::RInterpTo(GetActorRotation(), NewRot, DeltaSeconds, 10.f);
			SetActorRotation(NewRot);
		}
	}
}

float AC_NonPlayableCharacter::GetAIPatrolRadius()
{
	return 0.f;
}

float AC_NonPlayableCharacter::GetAIDetectRange()
{
	return EnemyTableRow->DetectRange;
}

float AC_NonPlayableCharacter::GetAIAttackRange()
{
	return EnemyTableRow->AttackRange;
}

float AC_NonPlayableCharacter::GetAITurnSpeed()
{
	return EnemyTableRow->TurnSpeed;
}

void AC_NonPlayableCharacter::SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AC_NonPlayableCharacter::Attack()
{
	check(EnemyTableRow);
	UAnimMontage* AttackAnim = EnemyTableRow->AttackAnim;
	
	PlayAnimMontage(AttackAnim);
	
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnMontageEnd);
	GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate);
}

void AC_NonPlayableCharacter::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	check(EnemyTableRow);
	UAnimMontage* AttackAnim = EnemyTableRow->AttackAnim;
	
	if (Montage == AttackAnim)
		OnAttackFinished.ExecuteIfBound();
	else if (Montage == TurnAnimMontage)
	{
		OnCharacterTurnFinished.ExecuteIfBound();
		TurnAnimMontage = nullptr;
		IsTurning = false;
	}
}

void AC_NonPlayableCharacter::SetAITurnFinishDelegate(const FAICharacterTurnFinished& InOnTurnFinished)
{
	OnCharacterTurnFinished = InOnTurnFinished;
}

void AC_NonPlayableCharacter::TurnInPlace(float TurnAnimDegree)
{
	check(EnemyTableRow);
	if (TurnAnimDegree == 90.f)
		TurnAnimMontage = EnemyTableRow->Left90TurnAnim;
	else if (TurnAnimDegree == 180.f)
		TurnAnimMontage = EnemyTableRow->Left180TurnAnim;
	else if (TurnAnimDegree == -90.f)
		TurnAnimMontage = EnemyTableRow->Right90TurnAnim;
	else if (TurnAnimDegree == -180.f)
		TurnAnimMontage = EnemyTableRow->Right180TurnAnim;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	check(AnimInstance);
	
	IsTurning = true;
	TurnStartYaw = GetActorRotation().Yaw;
	TurnDegree = TurnAnimDegree;
	
	PlayAnimMontage(TurnAnimMontage);
	
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnMontageEnd);
	GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate);
}
