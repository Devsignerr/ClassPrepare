// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "C_CharacterInterface.generated.h"

class UC_CharacterDataAsset;
class UC_CrowdControlComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UC_CharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTC_API IC_CharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool HasWeapon() = 0;
	virtual void AttackTrace(bool bStart, FName TraceStartBoneName, FName TraceEndBoneName) = 0;
	virtual void AttackTraceWithWeapon(bool bStart) = 0;
	
	virtual UStaticMeshComponent* GetStaticMeshComponent() = 0;
	virtual UC_CrowdControlComponent* GetCrowdControlComponent() = 0;
	virtual TPair<FName, FName> GetWeaponTraceNames() = 0;

	virtual UC_CharacterDataAsset* GetCharacterDataAsset() = 0;
	virtual bool IsDead() = 0;
};
