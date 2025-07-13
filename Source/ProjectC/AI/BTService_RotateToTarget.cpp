// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_RotateToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_RotateToTarget::UBTService_RotateToTarget()
{
	
}

void UBTService_RotateToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ACharacter* ControllingCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingCharacter)
		return;
	
	const APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
		return;

	FVector LookAtRot = (Target->GetActorLocation() - ControllingCharacter->GetActorLocation()).GetSafeNormal2D();

	const FRotator CurrentRot = ControllingCharacter->GetActorRotation();
	const FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot.Rotation(), DeltaSeconds, 10.f);
		
	ControllingCharacter->SetActorRotation(NewRot);
}
