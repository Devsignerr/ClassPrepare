// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_JumpBack.generated.h"

UCLASS()
class PROJECTC_API UBTTask_JumpBack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_JumpBack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UFUNCTION()
	void OnEndJump();

	UPROPERTY(EditAnywhere)
	float JumpPower = 2500.f;
	
	UPROPERTY(EditAnywhere)
	float LandWaitTime = 1.f;

	UPROPERTY(EditAnywhere)
	float JumpDegree = 45.f;

	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
	TWeakObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent = nullptr;
};
