// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectC/Data/C_CharacterStat.h"
#include "C_HUDWidget.generated.h"

UCLASS()
class PROJECTC_API UC_HUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UC_HUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	void UpdateStat(const FC_CharacterStat& BaseStat, const FC_CharacterStat& ModifierStat);
	void UpdateHpBar(float NewCurrentHp, float NewMaxHp);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UC_HpBarWidget> HpBarWidget;

	UPROPERTY()
	TObjectPtr<class UC_CharacterStatWidget> CharacterStatWidget;
};
