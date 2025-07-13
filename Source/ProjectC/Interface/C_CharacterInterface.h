// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectC/Character/Component/C_SkillComponent.h"
#include "C_CharacterInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FC_OnLandDelegate)

class FC_OnSkillMontageEnded;
class UC_BattleComponent;
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
	virtual void AttackTraceWithWeapon(bool bStart, bool bRight) = 0;
	
	virtual UStaticMeshComponent* GetWeapon_L_StaticMeshComponent() = 0;
	virtual UStaticMeshComponent* GetWeapon_R_StaticMeshComponent() = 0;
	virtual UC_CrowdControlComponent* GetCrowdControlComponent() = 0;
	virtual UC_SkillComponent* GetSkillComponent() const = 0;
	virtual UC_BattleComponent* GetBattleComponent() const = 0;
	virtual TPair<FName, FName> GetWeaponTraceNames(bool bRight) = 0;

	virtual UC_CharacterDataAsset* GetCharacterDataAsset() = 0;
	virtual bool IsDead() = 0;

	virtual FC_OnStartSkillDelegate& GetOnStartSkillDelegate() = 0;
	virtual FC_OnEndSkillDelegate& GetOnEndSkillDelegate() = 0;

	
	virtual FC_OnLandDelegate* GetOnLandedDelegate() = 0;
};
