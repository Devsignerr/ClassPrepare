// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_UserWidget.h"
#include "..\Data\C_TableRows.h"
#include "C_CharacterStatWidget.generated.h"

UCLASS()
class PROJECTC_API UC_CharacterStatWidget : public UC_UserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void UpdateStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat);

private:
	UPROPERTY()
	TMap<FName, class UTextBlock*> BaseLookup;

	UPROPERTY()
	TMap<FName, class UTextBlock*> ModifierLookup;
};
