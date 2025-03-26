// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerController.h"
#include "ProjectC/UI/C_HUDWidget.h"


AC_PlayerController::AC_PlayerController()
{
	static ConstructorHelpers::FClassFinder<UC_HUDWidget> HUDWidgetRef(TEXT("/Game/ArenC_attle/UI/WBP_C_HUD.WBP_C_HUD_C"));
	if (HUDWidgetRef.Class)
	{
		HUDWidgetClass = HUDWidgetRef.Class;
	}
}

void AC_PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AC_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void AC_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
