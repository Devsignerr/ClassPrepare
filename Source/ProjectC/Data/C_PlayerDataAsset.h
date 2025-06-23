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
struct FC_ComboAttackKey
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EC_CharacterStanceType CharacterStanceType = EC_CharacterStanceType::Sword;

	UPROPERTY(EditAnywhere)
	bool bSpecialAction = false;

	FC_ComboAttackKey()
	{
		
	}

	FC_ComboAttackKey(const EC_CharacterStanceType StanceType, bool SpecialAction)
	{
		CharacterStanceType = StanceType;
		bSpecialAction = SpecialAction;
	}

	bool operator==(const FC_ComboAttackKey& ComboAttackKey) const
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
struct FC_ComboEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FC_ComboAttackKey Key;

	UPROPERTY(EditAnywhere)
	FC_ComboAttackData Data;
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
	TArray<FC_ComboEntry> ComboAttackMontages; 

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> JumpSound;

	UPROPERTY(EditAnywhere)
	TMap<EC_CharacterStanceType, FC_SkillSlotData> SkillSlotDatas;
};
