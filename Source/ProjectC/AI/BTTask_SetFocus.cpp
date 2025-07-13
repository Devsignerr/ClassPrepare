// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetFocus.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


EBTNodeResult::Type UBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ACharacter* ControllingCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingCharacter)
		return EBTNodeResult::Failed;

	if (bFocus)
	{
		ControllingCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		ControllingCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	
	return EBTNodeResult::Succeeded;
}
