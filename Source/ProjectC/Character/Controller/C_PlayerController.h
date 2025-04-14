// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "C_PlayerController.generated.h"

UCLASS()
class PROJECTC_API AC_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AC_PlayerController();
	
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
};
