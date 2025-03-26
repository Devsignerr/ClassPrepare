// Fill out your copyright notice in the Description page of Project Settings.


#include "C_HUDWidget.h"
#include "C_HpBarWidget.h"
#include "C_CharacterStatWidget.h"
#include "ProjectC/Interface/C_CharacterHUDInterface.h"

UC_HUDWidget::UC_HUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UC_HUDWidget::UpdateStat(const FC_CharacterStat& BaseStat, const FC_CharacterStat& ModifierStat)
{
	FC_CharacterStat TotalStat = BaseStat + ModifierStat;
	CharacterStatWidget->UpdateStat(BaseStat, ModifierStat);
}

void UC_HUDWidget::UpdateHpBar(float NewCurrentHp, float NewMaxHp)
{
	HpBarWidget->UpdateHpBar(NewCurrentHp, NewMaxHp);
}

void UC_HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBarWidget = Cast<UC_HpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBarWidget);

	CharacterStatWidget = Cast<UC_CharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterStatWidget);
	
	if (IC_CharacterHUDInterface* HUDPawn = Cast<IC_CharacterHUDInterface>(GetOwningPlayerPawn()))
	{
		HUDPawn->SetupHUDWidget(this);
	}

}
