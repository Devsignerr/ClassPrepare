// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_UserWidget.h"
#include "C_HpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTC_API UC_HpBarWidget : public UC_UserWidget
{
	GENERATED_BODY()
	
public:
	UC_HpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateHpBar(float NewCurrentHp, float MaxHp);
	FString GetHPText();

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> PB_HP;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TB_HP;

	UPROPERTY()
	float CurrentHp;

	UPROPERTY()
	float MaxHp;
};
