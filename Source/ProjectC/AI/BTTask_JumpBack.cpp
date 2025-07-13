// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_JumpBack.h"
#include "AIController.h"
#include "C_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "ProjectC/Interface/C_CharacterInterface.h"

UBTTask_JumpBack::UBTTask_JumpBack()
{
	bCreateNodeInstance = true; 
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_JumpBack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ACharacter* ControllingCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingCharacter)
	{
		return EBTNodeResult::Failed;
	}

	IC_CharacterInterface* Interface = Cast<IC_CharacterInterface>(ControllingCharacter);
	if (!Interface)
	{
		return EBTNodeResult::Failed;
	}
	
	AC_AIController* AIController = Cast<AC_AIController>(ControllingCharacter->GetController());
	if (!AIController)
		return EBTNodeResult::Failed;

	const APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
		return EBTNodeResult::Failed;

	BehaviorTreeComponent = &OwnerComp; 
	
	FVector JumpDir = (ControllingCharacter->GetActorLocation() - Target->GetActorLocation()).GetSafeNormal2D();
	FVector Rightvector = ControllingCharacter->GetActorRightVector();	
	
	JumpDir = JumpDir.RotateAngleAxis(JumpDegree, Rightvector);
	JumpDir *= JumpPower;

	//DrawDebugDirectionalArrow(ControllingCharacter->GetWorld(), ControllingCharacter->GetActorLocation(),
	//	ControllingCharacter->GetActorLocation() + 100.f * JumpDir, 10.f, FColor::Red, false, 3.f);
	
	ControllingCharacter->LaunchCharacter(JumpDir, true, true);

	if (!Interface->GetOnLandedDelegate()->IsBoundToObject(this)) 
		Interface->GetOnLandedDelegate()->AddUObject(this, &ThisClass::OnEndJump);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_JumpBack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTask_JumpBack::OnEndJump()
{
	FTimerHandle LandTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(LandTimerHandle, [this]()
	{
		FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);

		IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
		if (CharacterInterface)
		{
			if (CharacterInterface->GetOnLandedDelegate()->IsBoundToObject(this))
				CharacterInterface->GetOnLandedDelegate()->RemoveAll(this);
		}
	}, LandWaitTime, false);
}
