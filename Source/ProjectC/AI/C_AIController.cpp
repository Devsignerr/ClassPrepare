// Fill out your copyright notice in the Description page of Project Settings.


#include "C_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectC/Character/C_NonPlayableCharacter.h"
#include "ProjectC/Utils/C_GameUtil.h"


void AC_AIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(TEXT("HomePose"), GetPawn()->GetActorLocation());

		bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AC_AIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AC_NonPlayableCharacter* NonPlayableCharacter = Cast<AC_NonPlayableCharacter>(GetPawn()))
	{
		if (const FC_EnemyTableRow* EnemyTableRow = FC_GameUtil::GetEnemyData(NonPlayableCharacter->CharacterType))
		{
			BBAsset = EnemyTableRow->BlackBoard;
			BTAsset = EnemyTableRow->BehaviorTree;
		}
	}

	RunAI();
}
