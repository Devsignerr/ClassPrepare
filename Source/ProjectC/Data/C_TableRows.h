
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "C_TableRows.generated.h"

USTRUCT(BlueprintType)
struct FC_CharacterStatTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FC_CharacterStatTableRow() : MaxHp(0.0f), Attack(0.0f), MovementSpeed(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
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