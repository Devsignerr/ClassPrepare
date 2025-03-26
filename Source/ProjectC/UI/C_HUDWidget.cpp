// Fill out your copyright notice in the Description page of Project Settings.


#include "C_HUDWidget.h"
#include "C_CharacterStatWidget.h"
#include "ProjectC/Interface/C_CharacterHUDInterface.h"

UC_HUDWidget::UC_HUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UC_HUDWidget::UpdateStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat)
{
	FC_CharacterStatTableRow TotalStat = BaseStat + ModifierStat;
	CharacterStatWidget->UpdateStat(BaseStat, ModifierStat);
}

void UC_HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterStatWidget = Cast<UC_CharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterStatWidget);
	
	if (IC_CharacterHUDInterface* HUDPawn = Cast<IC_CharacterHUDInterface>(GetOwningPlayerPawn()))
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
