// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckRange.generated.h"

UCLASS()
class PROJECTC_API UBTDecorator_CheckRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CheckRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere)
	float Range = 0.f;

	UPROPERTY(EditAnywhere)
	bool bCheckAttackRange = false;
};
