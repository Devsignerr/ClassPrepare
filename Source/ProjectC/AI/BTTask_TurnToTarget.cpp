// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
	bNotifyTick = true;
}

// ExecuteTask()는 Task 노드 실행의 진입점(entry point) 역할을 합니다.
// 이 함수가 호출되지 않으면 TickTask()도 절대 호출되지 않습니다
EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	const APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);

	if (!TargetPawn || !ControllingPawn || !AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
	LookVector.Z = 0.0f;
	const FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	const FRotator CurrentRot = ControllingPawn->GetActorRotation();

	// -180 ~ 180 정규화
	float YawDiff = FMath::UnwindDegrees(TargetRot.Yaw - CurrentRot.Yaw);
	
	if (YawDiff > 45.f && YawDiff < 135.f)
		AIPawn->TurnInPlace(90.f);
	else if (YawDiff >= 135.f)
		AIPawn->TurnInPlace(180.f);
	else if (YawDiff < -45.f && YawDiff > -135.f)
		AIPawn->TurnInPlace(-90.f);
	else if (YawDiff < -135.f)
		AIPawn->TurnInPlace(-180.f);
	else
	{
		Result = EBTNodeResult::Succeeded;
		return Result;
	}
	
	FAICharacterTurnFinished FaiCharacterTurnFinishDelegate;
	FaiCharacterTurnFinishDelegate.BindLambda(
		[&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);
	AIPawn->SetAITurnFinishDelegate(FaiCharacterTurnFinishDelegate);
	
	Result = EBTNodeResult::InProgress;
	return Result;
}

//즉시 회전시키는 코드. 버전 1 
// EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
// {
// 	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
//
// 	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
// 	if (!ControllingPawn)
// 		return EBTNodeResult::Failed;
//
// 	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
// 	if (!TargetPawn)
// 		return EBTNodeResult::Failed;
//
// 	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
// 	if (!AIPawn)
// 		return EBTNodeResult::Failed;
// 	
// 	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
// 	LookVector.Z = 0.0f;
// 	
// 	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
// 	ControllingPawn->SetActorRotation(TargetRot);
// 	
// 	return EBTNodeResult::Succeeded;
// }

// 천천히 회전시키는 코드. 버전 2
// void UBTTask_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
// {
// 	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
//
// 	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
// 	if (!ControllingPawn)
// 		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//
// 	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
// 	if (!TargetPawn)
// 		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//
// 	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
// 	if (!AIPawn)
// 		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//
// 	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
// 	LookVector.Z = 0.0f;
// 	const FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
// 	const FRotator CurrentRot = ControllingPawn->GetActorRotation();
//
// 	float TurnSpeed = AIPawn->GetAITurnSpeed();
// 	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaSeconds, TurnSpeed);
// 	ControllingPawn->SetActorRotation(NewRot);
//
// 	float YawDiff = FMath::UnwindDegrees(NewRot.Yaw - TargetRot.Yaw);
// 	if (FMath::Abs(YawDiff) < 45.f)
// 	{
// 		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
// 	}
// }

