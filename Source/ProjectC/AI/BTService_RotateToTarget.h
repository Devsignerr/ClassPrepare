// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RotateToTarget.generated.h"

UCLASS()
class PROJECTC_API UBTService_RotateToTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RotateToTarget();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
