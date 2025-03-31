
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "C_DataSubsystem.generated.h"

enum class EC_DataTableType;
class UDataTable;

UCLASS(config=Game)
class PROJECTC_API UC_DataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UC_DataSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UDataTable* GetTable(EC_DataTableType TableType);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EC_DataTableType, UDataTable*> DataTableMap;

private:
	UPROPERTY(config)
	FSoftObjectPath CharacterStat_DataTablePath;

	UPROPERTY(config)
	FSoftObjectPath Enemy_DataTablePath;
};
