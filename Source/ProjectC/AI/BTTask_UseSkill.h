// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UseSkill.generated.h"

UCLASS()
class PROJECTC_API UBTTask_UseSkill : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_UseSkill();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UFUNCTION()
	void OnEndSkill(uint32 InSkillId);

	UPROPERTY(EditAnywhere)
	uint32 SkillId = 0;

	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
	TWeakObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent = nullptr;
};
