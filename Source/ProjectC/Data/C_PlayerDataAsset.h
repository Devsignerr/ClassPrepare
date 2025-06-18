// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectC/enums.h"
#include "C_PlayerDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FC_SkillSlotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<EC_SkillSlotType, uint32> SkillIds;
};

UCLASS()
class PROJECTC_API UC_PlayerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed_Walk;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed_Jog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed_Sprint;
	
	UPROPERTY(EditAnywhere)
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> JumpSound;

	UPROPERTY(EditAnywhere)
	TMap<EC_CharacterStanceType, FC_SkillSlotData> SkillSlotDatas;
};
