// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveAlongPatrolRoute.h"
#include "AIController.h"
#include "C_AIController.h"
#include "Actor/C_PatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Character/C_NonPlayableCharacter.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"

EBTNodeResult::Type UBTTask_MoveAlongPatrolRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingPawn)
		return EBTNodeResult::Failed;

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
		return EBTNodeResult::Failed;

	AC_AIController* AIController = Cast<AC_AIController>(ControllingPawn->GetController());
	if (!AIController)
		return EBTNodeResult::Failed;

	AC_PatrolRoute* PatrolRoute = Cast<AC_PatrolRoute>(AIPawn->GetPatrolRoute());
	if (PatrolRoute)
	{
		FVector TargetLocation = PatrolRoute->GetLocationAtSplinePoint();
		AIController->MoveToLocation(TargetLocation, 1.f, false);
	}
	else
	{
		FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("HomePos"));
		AIController->MoveToLocation(TargetLocation, 1.f, false);
	}
	
	return EBTNodeResult::Succeeded;
}
