// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "ProjectC/Character/C_PlayableCharacter.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
		return;
	
	UWorld* World = ControllingPawn->GetWorld();
	if (!World)
		return;

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
		return;

	float DetectRadius = AIPawn->GetAIDetectRange();
	FVector PawnLocation = ControllingPawn->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam;
	CollisionQueryParam.bTraceComplex = false;
	CollisionQueryParam.AddIgnoredActor(ControllingPawn);
	
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		PawnLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			if (AC_PlayableCharacter* Player = Cast<AC_PlayableCharacter>(OverlapResult.GetActor()))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Player);
				AIPawn->ChangeState(EC_EnemyStateType::Battle);
				
				DrawDebugSphere(World, PawnLocation, DetectRadius, 16, FColor::Green, false, 0.2f);

				DrawDebugPoint(World, Player->GetActorLocation(), 10.0f, FColor::Green, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Player->GetActorLocation(), FColor::Green, false, 0.27f);
				return;
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);

	if (AIPawn->GetState() != EC_EnemyStateType::Patrol)
		AIPawn->ChangeState(EC_EnemyStateType::Patrol);
	
	DrawDebugSphere(World, PawnLocation, DetectRadius, 16, FColor::Red, false, 0.2f);
}


