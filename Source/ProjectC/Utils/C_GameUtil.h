#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectC/enums.h"
#include "ProjectC/Subsystem/C_DataSubsystem.h"

class UC_PlayerDataAsset;
class UNiagaraComponent;
struct FC_CrowdControlTableRow;
class UNiagaraSystem;
struct FC_ExecTableRow;
struct FC_SkillObjectTableRow;
struct FC_SkillTableRow;
struct FC_WeaponTableRow;
struct FC_EnemyTableRow;
struct FC_CharacterStatTableRow;

class PROJECTC_API FC_GameUtil
{
public:
	static FC_CharacterStatTableRow* GetCharacterStatData(uint32 DataId);
	static FC_EnemyTableRow* GetEnemyData(uint32 EnemyId);
	static FC_WeaponTableRow* GetWeaponData(uint8 WeaponId);
	static FC_SkillTableRow* GetSkillData(uint32 SkillId);
	static FC_ExecTableRow* GetExecData(uint32 ExecId);
	static FC_SkillObjectTableRow* GetSkillObjectData(uint32 SkillObjectId);
	static FC_CrowdControlTableRow* GetCrowdControlData(uint32 CrowdControlId);
	
	static UC_CameraDataAsset* GetCameraData(EC_CameraType CameraType);

	static ECollisionChannel GetAttackCollisionChannel(uint32 DataId);
	
	static void CameraShake();
	static void SpawnEffectAtLocation(UObject* WorldContextObj, UNiagaraSystem* NiagaraSystem, FVector Location, FRotator Rotation);
	static void SpawnEffectAtLocation(UObject* WorldContextObj, UParticleSystem* ParticleSystem, FVector Location, FRotator Rotation);

	static UNiagaraComponent* SpawnEffectAttached(UNiagaraSystem* NiagaraSystem, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy);

	static TArray<UAnimMontage*> GetComboAttackMontages(UC_PlayerDataAsset* PlayerDataAsset, EC_CharacterStanceType StanceType, bool bInSpecialAttack);
	static uint32 GetSkillId(UC_PlayerDataAsset* PlayerDataAsset, EC_SkillSlotType SkillSlotType, EC_CharacterStanceType StanceType, bool bInSpecialAttack);

	static FVector FindSurfacePos(ACharacter* Character, FVector& CurrentPos);
	static void PlayHitStop(UObject* WorldObject, float Duration, float Dilation);
	
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


