
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "C_TableRows.generated.h"

USTRUCT(BlueprintType)
struct FC_CharacterStatTableRow : public FTableRowBase
{
	GENERATED_BODY()

	FC_CharacterStatTableRow() : MaxHp(0.0f), Attack(0.0f), MovementSpeed(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EC_CharacterType CharacterType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;

	FC_CharacterStatTableRow operator+(const FC_CharacterStatTableRow& Other) const
	{
		FC_CharacterStatTableRow Result;
		
		Result.MaxHp = this->MaxHp + Other.MaxHp;
		Result.Attack = this->Attack + Other.Attack;
		Result.MovementSpeed = this->MovementSpeed + Other.MovementSpeed;

		return Result;
	}
};


USTRUCT(BlueprintType)
struct FC_EnemyTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EC_CharacterType EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBlackboardData> BlackBoard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectRange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 0.f;
};