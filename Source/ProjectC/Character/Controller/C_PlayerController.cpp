// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PlayerController.h"

#include "ProjectC/Subsystem/C_UISubsystem.h"
#include "ProjectC/UI/C_HUDWidget.h"


AC_PlayerController::AC_PlayerController()
{
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

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UC_UISubsystem* UISubsystem = GameInstance->GetSubsystem<UC_UISubsystem>())
		{
			UISubsystem->CreateHUD();
		}
	}
}

void AC_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
