// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_ChoosePattern.h"
#include "AIController.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"

UBTDecorator_ChoosePattern::UBTDecorator_ChoosePattern()
{
	NodeName = TEXT("ChoosePattern");
}

bool UBTDecorator_ChoosePattern::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
		return false;

	const IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
		return false;

	uint32 RandomValue = FMath::RandRange(0, 100);
	if (RandomValue <= Percent)
	{
		return true;
	}
	
	return false;
}
