// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "C_CharacterDataAsset.generated.h"

UCLASS()
class PROJECTC_API UC_CharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> KnockbackAnim;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> DeathAnim;
};
