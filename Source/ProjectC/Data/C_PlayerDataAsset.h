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

USTRUCT(BlueprintType)
struct FC_ComboKey
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EC_CharacterStanceType CharacterStanceType = EC_CharacterStanceType::Sword;

	UPROPERTY(EditAnywhere)
	bool bSpecialAction = false;

	FC_ComboKey(){}

	FC_ComboKey(const EC_CharacterStanceType StanceType, bool SpecialAction)
	{
		CharacterStanceType = StanceType;
		bSpecialAction = SpecialAction;
	}

	bool operator==(const FC_ComboKey& ComboAttackKey) const
	{
		return ComboAttackKey.CharacterStanceType == CharacterStanceType && ComboAttackKey.bSpecialAction == bSpecialAction;
	}
};

USTRUCT(BlueprintType)
struct FC_ComboAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;
};

USTRUCT(BlueprintType)
struct FC_ComboAttackEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FC_ComboKey Key;

	UPROPERTY(EditAnywhere)
	FC_ComboAttackData Data;
};

USTRUCT(BlueprintType)
struct FC_SkillEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FC_ComboKey Key;

	UPROPERTY(EditAnywhere)
	FC_SkillSlotData Data;
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
	TArray<FC_ComboAttackEntry> ComboAttackMontages; 

	UPROPERTY(EditAnywhere)
	TArray<FC_SkillEntry> SkillSlotDatas;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> JumpSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> DamageMaterial = nullptr;
};
