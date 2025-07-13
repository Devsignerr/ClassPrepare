// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetState.h"
#include "AIController.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"


EBTNodeResult::Type UBTTask_SetState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	AIPawn->RequestChangeState(StateType);
	
	return EBTNodeResult::Succeeded;
}
