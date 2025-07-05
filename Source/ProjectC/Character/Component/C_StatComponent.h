// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "..\..\Data\C_TableRows.h"
#include "C_StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHpChangedDelegate, float /*CurrentHp*/, float /*MaxHp*/);
DECLARE_MULTICAST_DELEGATE(FC_OnDeadDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FC_CharacterStatTableRow& /*BaseStat*/, const FC_CharacterStatTableRow& /*ModifierStat*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_StatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_StatComponent();

protected:
	virtual void InitializeComponent() override;

public:
	FOnHpChangedDelegate OnHpChanged;
	FC_OnDeadDelegate OnDead;
	FOnStatChangedDelegate OnStatChanged;
	
	void AddBaseStat(const FC_CharacterStatTableRow& InAddBaseStat);
	void SetBaseStat(const FC_CharacterStatTableRow& InBaseStat);
	void SetModifierStat(const FC_CharacterStatTableRow& InModifierStat);
	
	const FC_CharacterStatTableRow& GetBaseStat() const { return BaseStat; }
	const FC_CharacterStatTableRow& GetModifierStat() const { return ModifierStat; }
	FC_CharacterStatTableRow GetTotalStat() const { return BaseStat + ModifierStat; }
	
	float GetCurrentHp() const { return CurrentHp; }
	float GetMaxHp() const { return MaxHp; }
	
	void HealHp(float InHealAmount);
	float ApplyDamage(float InDamage, AActor* DamageCauser);

public:
	void SetHp(float NewHp);

	float MaxHp;
	float CurrentHp;

	UPROPERTY(Meta = (AllowPrivateAccess = "true"))
	FC_CharacterStatTableRow BaseStat;

	UPROPERTY(Meta = (AllowPrivateAccess = "true"))
	FC_CharacterStatTableRow ModifierStat;

public:
	void ResetStat();
};
