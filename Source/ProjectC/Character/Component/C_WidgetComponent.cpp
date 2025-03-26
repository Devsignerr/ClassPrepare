// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WidgetComponent.h"
#include "ProjectC/UI/C_UserWidget.h"

void UC_WidgetComponent::InitWidget()
{
	Super::InitWidget();
	
	if (UC_UserWidget* UserWidget = Cast<UC_UserWidget>(GetWidget()))
	{
		UserWidget->SetOwningActor(GetOwner());
	}
}
