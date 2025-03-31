#include "C_GameUtil.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "ProjectC/ProjectC.h"
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
