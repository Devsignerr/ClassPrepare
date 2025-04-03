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
	virtual void Tick(float DeltaSeconds) override;

	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void ResetState() override;
	virtual void ChangeState(EC_EnemyStateType StateType) override;
	
	virtual AActor* GetPatrolRoute() override;

	virtual void SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void Attack() override;
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	virtual void SetAITurnFinishDelegate(const FAICharacterTurnFinished& InOnTurnFinished) override;
	virtual void TurnInPlace(float TurnAnimDegree) override;

public:
	UFUNCTION(BlueprintCallable) 
	virtual EC_EnemyStateType GetState() override;
	
	UFUNCTION()
	virtual void IncrementPatrolIndex() override;

private:
	FAICharacterAttackFinished OnAttackFinished;
	FAICharacterTurnFinished OnCharacterTurnFinished;
	
	FC_EnemyTableRow* EnemyTableRow = nullptr;

	UPROPERTY()
	TObjectPtr<UAnimMontage> TurnAnimMontage = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> PatrolRoute = nullptr;

	EC_EnemyStateType EnemyState = EC_EnemyStateType::None;

	bool IsTurning = false;

	float TurnStartYaw = 0.f;
	float TurnDegree = 0.f;
};

