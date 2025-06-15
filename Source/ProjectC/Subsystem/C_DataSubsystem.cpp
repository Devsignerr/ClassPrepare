
#include "C_DataSubsystem.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "ProjectC/Data/C_CameraDataAsset.h"

UC_DataSubsystem::UC_DataSubsystem()
{

}

void UC_DataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<TPair<EC_DataTableType, FSoftObjectPath>> LoadList;
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::CharacterStat, CharacterStat_DataTablePath));
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::Enemy, Enemy_DataTablePath));
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::Weapon, Weapon_DataTablePath));
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::Skill, Skill_DataTablePath));
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::Exec, Exec_DataTablePath));
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::SkillObject, SkillObject_DataTablePath));
	
	for (const auto& Element : LoadList)
	{
		const FSoftObjectPath DataTableAssetPath(Element.Value);
		if (DataTableAssetPath.IsValid())
		{
			if (UDataTable* Table = Cast<UDataTable>(DataTableAssetPath.TryLoad()))
			{
				DataTableMap.Add(Element.Key, Table);
			}
		}
	}
	
	const FSoftObjectPath NormalCameraAssetObject(NormalCameraDataPath);
	if (NormalCameraAssetObject.IsValid())
	{
		if (UC_CameraDataAsset* DataAsset = Cast<UC_CameraDataAsset>(NormalCameraAssetObject.TryLoad()))
		{
			CameraData.Add(EC_CameraType::Normal, DataAsset);
		}
	}

	const FSoftObjectPath AimCameraAssetObject(AimCameraDataPath);
	if (AimCameraAssetObject.IsValid())
	{
		if (UC_CameraDataAsset* DataAsset = Cast<UC_CameraDataAsset>(AimCameraAssetObject.TryLoad()))
		{
			CameraData.Add(EC_CameraType::Aim, DataAsset);
		}
	}
}

UDataTable* UC_DataSubsystem::GetTable(EC_DataTableType TableType)
{
	if (DataTableMap.Contains(TableType))
	{
		return DataTableMap[TableType];
	}

	return nullptr;
}
