#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectC/enums.h"
#include "ProjectC/Subsystem/C_DataSubsystem.h"

struct FC_ExecTableRow;
struct FC_SkillObjectTableRow;
struct FC_SkillTableRow;
struct FC_WeaponTableRow;
struct FC_EnemyTableRow;
struct FC_CharacterStatTableRow;

class PROJECTC_API FC_GameUtil
{
public:
	static FC_CharacterStatTableRow* GetCharacterStatData(EC_CharacterType CharacterType);
	static FC_EnemyTableRow* GetEnemyData(EC_CharacterType EnemyType);
	static FC_WeaponTableRow* GetWeaponData(uint8 WeaponId);
	static UC_CameraDataAsset* GetCameraData(EC_CameraType CameraType);
	static FC_SkillTableRow* GetSkillData(uint32 SkillId);
	static FC_ExecTableRow* GetExecData(uint32 ExecId);
	static FC_SkillObjectTableRow* GetSkillObjectData(uint32 SkillObjectId);

	static void CameraShake();
	
	template <typename T>
	static TArray<T*> GetAllRows(EC_DataTableType DataTableType);
};

template <typename T>
TArray<T*> FC_GameUtil::GetAllRows(EC_DataTableType DataTableType)
{
	if (GEngine)
	{
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld()))
		{
			if (UC_DataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UC_DataSubsystem>())
			{
				if (const UDataTable* DataTable = DataSubsystem->GetTable(DataTableType))
				{
					TArray<T*> TableRows;
					DataTable->GetAllRows(TEXT(""), TableRows);

					return TableRows;
				}
			}
		}
	}

	return TArray<T*>();
}


