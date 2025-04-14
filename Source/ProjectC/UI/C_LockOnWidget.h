// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_UserWidget.h"
#include "C_LockOnWidget.generated.h"

class UImage;

UCLASS()
class PROJECTC_API UC_LockOnWidget : public UC_UserWidget
{
	GENERATED_BODY()
	
public:
	UC_LockOnWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void ToggleActivation(bool bEnable);     
      
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_LockOn;
};
