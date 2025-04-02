// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "C_CharacterAIInterface.generated.h"

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
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	virtual void TurnInPlace(float TurnAnimDegree) = 0;
	virtual void SetAITurnFinishDelegate(const FAICharacterTurnFinished& InOnTurnFinished) = 0;

	virtual void SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished) = 0;
	virtual void Attack() = 0;
};
