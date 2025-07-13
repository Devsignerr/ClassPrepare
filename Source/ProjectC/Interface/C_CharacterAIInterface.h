// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectC/enums.h"
#include "UObject/Interface.h"
#include "C_CharacterAIInterface.generated.h"

struct FC_EnemyTableRow;

DECLARE_DELEGATE(FAICharacterAttackFinished);
DECLARE_DELEGATE(FAICharacterTurnFinished);

UINTERFACE(MinimalAPI)
class UC_CharacterAIInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTC_API IC_CharacterAIInterface
{
	GENERATED_BODY()

public:
	virtual FC_EnemyTableRow* GetEnemyData() = 0;

	virtual void ResetState() = 0;
	virtual void RequestChangeState(EC_EnemyStateType StateType) = 0;
	virtual EC_EnemyStateType GetState() = 0;

	virtual AActor* GetPatrolRoute() = 0;
	virtual void IncrementPatrolIndex() = 0;

	virtual void TurnInPlace(float TurnAnimDegree) = 0;
	virtual void SetAITurnFinishDelegate(const FAICharacterTurnFinished& InOnTurnFinished) = 0;

	virtual void SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished) = 0;
	virtual void Attack() = 0;
};
