// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsLookingTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"

UBTDecorator_IsLookingTarget::UBTDecorator_IsLookingTarget()
{
	NodeName = TEXT("IsLookingTarget");
}

bool UBTDecorator_IsLookingTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
		return false;

	const IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
		return false;

	const APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
		return false;
	
	const FVector Forward = ControllingPawn->GetActorForwardVector();
	const FVector ToTarget = (Target->GetActorLocation() - ControllingPawn->GetActorLocation()).GetSafeNormal();
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(Forward.Dot(ToTarget)));

	bResult = Angle < 45.f;
	
	return bResult;
}
