// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "C_CharacterBase.h"
#include "ProjectC/Interface/C_CharacterAIInterface.h"
#include "C_NonPlayableCharacter.generated.h"

UCLASS(config=Game)
class AC_NonPlayableCharacter : public AC_CharacterBase, public IC_CharacterAIInterface
{
	GENERATED_BODY()

public:
	AC_NonPlayableCharacter();

protected:
	virtual void BeginPlay() override;

	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void Attack() override;
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

private:
	FAICharacterAttackFinished OnAttackFinished;
	
	FC_EnemyTableRow* EnemyTableRow = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AttackAnim = nullptr;
};

