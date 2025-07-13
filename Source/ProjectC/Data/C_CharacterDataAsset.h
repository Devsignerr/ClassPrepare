// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "C_CharacterDataAsset.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FC_WeaponData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintType, EditAnywhere)
	int32 WeaponId_L = -1;

	UPROPERTY(BlueprintType, EditAnywhere)
	int32 WeaponId_R = -1;
};

UCLASS()
class PROJECTC_API UC_CharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> HitFX;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> GuardFX;

	UPROPERTY(EditAnywhere)
	FName WeaponSocketName_L;

	UPROPERTY(EditAnywhere)
	FName WeaponSocketName_R;

	UPROPERTY(EditAnywhere)
	TArray<FC_WeaponData> WeaponIds;
 };
