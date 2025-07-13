// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectC/Data/C_TableRows.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"


EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ACharacter* ControllingCharacter = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControllingCharacter)
		return EBTNodeResult::Failed;

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(ControllingCharacter);
	if (!AIPawn)
		return EBTNodeResult::Failed;

	FC_EnemyTableRow* EnemyTableRow = AIPawn->GetEnemyData();
	ensure(EnemyTableRow);

	float MoveSpeed = 0.f;
	
	if (EC_MovementType::Walk == MovementType)
		MoveSpeed = EnemyTableRow->MovementSpeed_Walk;
	else if (EC_MovementType::Run == MovementType)
		MoveSpeed = EnemyTableRow->MovementSpeed_Run;
	else if (EC_MovementType::Strafe == MovementType)
		MoveSpeed = EnemyTableRow->MovementSpeed_Strafe;

	ControllingCharacter->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	
	return EBTNodeResult::Succeeded;
}
