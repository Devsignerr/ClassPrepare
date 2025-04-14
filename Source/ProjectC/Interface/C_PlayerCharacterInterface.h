// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "C_PlayerCharacterInterface.generated.h"

class UC_ActionComponent;
class UC_BattleComponent;
class UC_PlayerDataAsset;
class UC_LockOnComponent;

UINTERFACE(MinimalAPI)
class UC_PlayerCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTC_API IC_PlayerCharacterInterface
{
	GENERATED_BODY()

public:
	virtual UStaticMeshComponent* GetWeaponStaticMeshComponent() const = 0;
	virtual UC_ActionComponent* GetActionComponent() const = 0;
	virtual UC_LockOnComponent* GetLockOnComponent() const = 0;
	virtual UC_BattleComponent* GetBattleComponent() const = 0;
	virtual UC_PlayerDataAsset* GetPlayerData() const = 0;
};
