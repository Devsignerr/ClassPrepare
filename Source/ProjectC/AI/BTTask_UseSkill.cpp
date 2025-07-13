// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UseSkill.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "ProjectC/Character/Component/C_SkillComponent.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"
#include "ProjectC/Interface/C_CharacterInterface.h"

UBTTask_UseSkill::UBTTask_UseSkill()
{
	bCreateNodeInstance = true; 
	bNotifyTick= false;
}

EBTNodeResult::Type UBTTask_UseSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingPawn)
		return EBTNodeResult::Failed;

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
		return EBTNodeResult::Failed;

	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(ControllingPawn);
	check(CharacterInterface);

	UC_SkillComponent* SkillComponent = CharacterInterface->GetSkillComponent();
	check(SkillComponent);
	
	BehaviorTreeComponent = &OwnerComp;
	OwnerCharacter = Cast<ACharacter>(ControllingPawn);

	FC_OnEndSkillDelegate& OnEndSkillDelegate = CharacterInterface->GetOnEndSkillDelegate();

	if (!OnEndSkillDelegate.IsAlreadyBound(this, &ThisClass::OnEndSkill))
		OnEndSkillDelegate.AddDynamic(this, &ThisClass::OnEndSkill);

	SkillComponent->RequestPlaySkill(SkillId);

	return EBTNodeResult::InProgress;
}

void UBTTask_UseSkill::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_UseSkill::OnEndSkill(uint32 InSkillId)
{
	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
	if (CharacterInterface)
	{
		FC_OnEndSkillDelegate& OnEndSkillDelegate = CharacterInterface->GetOnEndSkillDelegate();

		if (OnEndSkillDelegate.IsAlreadyBound(this, &ThisClass::OnEndSkill))
			OnEndSkillDelegate.RemoveDynamic(this, &ThisClass::OnEndSkill);
	}
	
	if (InSkillId == SkillId)
	{
		if (BehaviorTreeComponent.Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("[UseSkill] FinishLatentTask 호출 전, BTComp=%s"), 
				   *BehaviorTreeComponent->GetName());

			FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UseSkill] BehaviorTreeComponent 포인터가 널입니다!"));
		}
	}
}
