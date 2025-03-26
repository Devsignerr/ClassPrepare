// Fill out your copyright notice in the Description page of Project Settings.


#include "C_HpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ProjectC/Interface/C_CharacterWidgetInterface.h"

UC_HpBarWidget::UC_HpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

void UC_HpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PB_HP = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(PB_HP);

	TB_HP = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtTB_HP")));
	ensure(TB_HP);

	IC_CharacterWidgetInterface* CharacterWidget = Cast<IC_CharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UC_HpBarWidget::UpdateHpBar(float NewCurrentHp, float NewMaxHp)
{
	CurrentHp = NewCurrentHp;
	MaxHp = NewMaxHp;

	if (MaxHp <= 0.f)
		return;
	
	if (PB_HP)
	{
		PB_HP->SetPercent(CurrentHp / MaxHp);
	}

	if (TB_HP)
	{
		TB_HP->SetText(FText::FromString(GetHPText()));
	}
}

FString UC_HpBarWidget::GetHPText()
{
	return FString::Printf(TEXT("%.0f/%0.f"), CurrentHp, MaxHp);
}
