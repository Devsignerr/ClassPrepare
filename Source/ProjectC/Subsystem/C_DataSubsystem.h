
#pragma once

#include "CoreMinimal.h"
#include "ProjectC/enums.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "C_DataSubsystem.generated.h"

class UC_CameraDataAsset;
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

	UPROPERTY()
	TMap<EC_CameraType, UC_CameraDataAsset*> CameraData;
	
private:
	UPROPERTY(config)
	FSoftObjectPath CharacterStat_DataTablePath;

	UPROPERTY(config)
	FSoftObjectPath Enemy_DataTablePath;

	UPROPERTY(config)
	FSoftObjectPath Weapon_DataTablePath;

	UPROPERTY(config)
	FSoftObjectPath Skill_DataTablePath;

	UPROPERTY(config)
	FSoftObjectPath SkillObject_DataTablePath;

	UPROPERTY(config)
	FSoftObjectPath NormalCameraDataPath;

	UPROPERTY(config)
	FSoftObjectPath AimCameraDataPath;
};
