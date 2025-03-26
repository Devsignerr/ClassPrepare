// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_UserWidget.h"
#include "..\Data\C_TableRows.h"
#include "C_HUDWidget.generated.h"

UCLASS()
class PROJECTC_API UC_HUDWidget : public UC_UserWidget
{
	GENERATED_BODY()

public:
	UC_HUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	void UpdateStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UC_CharacterStatWidget> CharacterStatWidget;
};
