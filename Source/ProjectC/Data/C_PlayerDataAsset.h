// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "C_PlayerDataAsset.generated.h"

UCLASS()
class PROJECTC_API UC_PlayerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed_Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed_Run;

	UPROPERTY(EditAnywhere)
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> JumpSound;
};
