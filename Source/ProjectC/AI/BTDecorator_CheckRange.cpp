// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectC/Data/C_TableRows.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"

UBTDecorator_CheckRange::UBTDecorator_CheckRange()
{
	NodeName = TEXT("CheckRange");
}

bool UBTDecorator_CheckRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
		return false;

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
		return false;

	const APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
		return false;

	FC_EnemyTableRow* EnemyTableRow = AIPawn->GetEnemyData();
	ensure(EnemyTableRow);
 
	const float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	const float AttackRangeWithRadius = EnemyTableRow->AttackRange;

	if (bCheckAttackRange)
		bResult = DistanceToTarget <= AttackRangeWithRadius;
	else
	{
		bResult = DistanceToTarget <= Range;
	}
	
	return bResult;
}
