
#include "C_NonPlayableCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
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
	ensure(EnemyTableRow);
	
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetSkeletalMesh(EnemyTableRow->SkeletalMesh);
		MeshComponent->SetAnimClass(EnemyTableRow->AnimInstance);
	}

	GetCharacterMovement()->MaxWalkSpeed = EnemyTableRow->MovementSpeed;
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
	return 0.f;
}

void AC_NonPlayableCharacter::SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AC_NonPlayableCharacter::Attack()
{
	PlayAnimMontage(AttackAnim);
	
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnMontageEnd);
	GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate);
}

void AC_NonPlayableCharacter::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackAnim)
		OnAttackFinished.ExecuteIfBound();
}
