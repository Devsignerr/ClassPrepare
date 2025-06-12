#include "C_GameUtil.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Cosmetic/C_CameraShake.h"
#include "ProjectC/Data/C_TableRows.h"

FC_CharacterStatTableRow* FC_GameUtil::GetCharacterStatData(EC_CharacterType CharacterType)
{
	TArray<FC_CharacterStatTableRow*> CharacterTableRows = GetAllRows<FC_CharacterStatTableRow>(EC_DataTableType::CharacterStat);
	if (FC_CharacterStatTableRow** FoundRow = CharacterTableRows.FindByPredicate([CharacterType](const FC_CharacterStatTableRow* Row)
	{
		return Row->CharacterType == CharacterType;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("CharacterStatData is Invalid"));
	return nullptr;
}

FC_EnemyTableRow* FC_GameUtil::GetEnemyData(EC_CharacterType EnemyType)
{
	TArray<FC_EnemyTableRow*> EnemyTableRows = GetAllRows<FC_EnemyTableRow>(EC_DataTableType::Enemy);
	if (FC_EnemyTableRow** FoundRow = EnemyTableRows.FindByPredicate([EnemyType](const FC_EnemyTableRow* Row)
	{
		return Row->EnemyType == EnemyType;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("EnemyData is Invalid"));
	return nullptr;
}

FC_WeaponTableRow* FC_GameUtil::GetWeaponData(uint8 WeaponId)
{
	TArray<FC_WeaponTableRow*> EnemyTableRows = GetAllRows<FC_WeaponTableRow>(EC_DataTableType::Weapon);
	if (FC_WeaponTableRow** FoundRow = EnemyTableRows.FindByPredicate([WeaponId](const FC_WeaponTableRow* Row)
	{
		return Row->WeaponId == WeaponId;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("WeaponData is Invalid"));
	return nullptr;
}

UC_CameraDataAsset* FC_GameUtil::GetCameraData(EC_CameraType CameraType)
{
	if (GEngine)
	{
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld()))
		{
			if (UC_DataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UC_DataSubsystem>())
			{
				return *DataSubsystem->CameraData.Find(CameraType);
			}
		}
	}

	return nullptr;
}

FC_SkillTableRow* FC_GameUtil::GetSkillData(uint32 SkillId)
{
	TArray<FC_SkillTableRow*> EnemyTableRows = GetAllRows<FC_SkillTableRow>(EC_DataTableType::Skill);
	if (FC_SkillTableRow** FoundRow = EnemyTableRows.FindByPredicate([SkillId](const FC_SkillTableRow* Row)
	{
		return Row->DataId == SkillId;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("SkillData is Invalid"));
	return nullptr;
}

FC_SkillObjectTableRow* FC_GameUtil::GetSkillObjectData(uint32 SkillObjectId)
{
	TArray<FC_SkillObjectTableRow*> EnemyTableRows = GetAllRows<FC_SkillObjectTableRow>(EC_DataTableType::SkillObject);
	if (FC_SkillObjectTableRow** FoundRow = EnemyTableRows.FindByPredicate([SkillObjectId](const FC_SkillObjectTableRow* Row)
	{
		return Row->DataId == SkillObjectId;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("SkillObjectData is Invalid"));
	return nullptr;
}

void FC_GameUtil::CameraShake()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GEngine->GetCurrentPlayWorld(), 0))
	{
		PlayerController->ClientStartCameraShake(UC_CameraShake::StaticClass());
	}
}
