// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectC/Data/C_CharacterStat.h"
#include "C_CharacterStatWidget.generated.h"

UCLASS()
class PROJECTC_API UC_CharacterStatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void UpdateStat(const FC_CharacterStat& BaseStat, const FC_CharacterStat& ModifierStat);

private:
	UPROPERTY()
	TMap<FName, class UTextBlock*> BaseLookup;

	UPROPERTY()
	TMap<FName, class UTextBlock*> ModifierLookup;
};
