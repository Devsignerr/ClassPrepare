
#include "C_DataSubsystem.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"

UC_DataSubsystem::UC_DataSubsystem()
{

}

void UC_DataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<TPair<EC_DataTableType, FSoftObjectPath>> LoadList;
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::CharacterStat, CharacterStat_DataTablePath));
	LoadList.Add(TPair<EC_DataTableType, FSoftObjectPath>(EC_DataTableType::Enemy, Enemy_DataTablePath));
	
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
}

UDataTable* UC_DataSubsystem::GetTable(EC_DataTableType TableType)
{
	if (DataTableMap.Contains(TableType))
	{
		return DataTableMap[TableType];
	}

	return nullptr;
}
