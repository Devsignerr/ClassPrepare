// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ProjectC/enums.h"
#include "BTTask_SetMovementSpeed.generated.h"


UCLASS()
class PROJECTC_API UBTTask_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	EC_MovementType MovementType = EC_MovementType::None;
};
 