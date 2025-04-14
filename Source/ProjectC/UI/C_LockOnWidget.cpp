// Fill out your copyright notice in the Description page of Project Settings.


#include "C_LockOnWidget.h"

#include "ProjectC/Interface/C_CharacterWidgetInterface.h"

UC_LockOnWidget::UC_LockOnWidget(const FObjectInitializer& ObjectInitializer)
{
	
}

void UC_LockOnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	IC_CharacterWidgetInterface* CharacterWidget = Cast<IC_CharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupLockOnWidget(this);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UC_LockOnWidget::ToggleActivation(bool bEnable)
{
	if (bEnable)
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else
		SetVisibility(ESlateVisibility::Collapsed);
}

