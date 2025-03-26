#include "C_GameUtil.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectC/enums.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Data/C_TableRows.h"
#include "ProjectC/Subsystem/C_DataSubsystem.h"

FC_CharacterStatTableRow* FC_GameUtil::GetCharacterStatData(EC_CharacterType CharacterType)
{
	if (GEngine)
	{
		if (UGameInstance* GameInstance =UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld()))
		{
			if (UC_DataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UC_DataSubsystem>())
			{
				if (const UDataTable* DataTable = DataSubsystem->GetTable(EC_DataTableType::CharacterStat))
				{
					TArray<FC_CharacterStatTableRow*> CharacterTableRows;
					DataTable->GetAllRows(TEXT(""), CharacterTableRows);

					if (FC_CharacterStatTableRow** FoundRow = CharacterTableRows.FindByPredicate([CharacterType](const FC_CharacterStatTableRow* Row)
					{
						return Row->CharacterType == CharacterType;
					}))
					{
						return *FoundRow;
					}
				}
			}
		}
	}
	
	UE_LOG(LogProjectC, Error, TEXT("CharacterStatData is Invalid"));
	return nullptr;
}
