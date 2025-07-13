#include "C_GameUtil.h"
#include "Containers/Ticker.h"
#include "NavigationSystem.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Cosmetic/C_CameraShake.h"
#include "ProjectC/Data/C_TableRows.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "ProjectC/Data/C_PlayerDataAsset.h"

FC_CharacterStatTableRow* FC_GameUtil::GetCharacterStatData(uint32 DataId)
{
	TArray<FC_CharacterStatTableRow*> CharacterTableRows = GetAllRows<FC_CharacterStatTableRow>(EC_DataTableType::CharacterStat);
	if (FC_CharacterStatTableRow** FoundRow = CharacterTableRows.FindByPredicate([DataId](const FC_CharacterStatTableRow* Row)
	{
		return Row->DataId == DataId;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("CharacterStatData is Invalid"));
	return nullptr;
}

FC_EnemyTableRow* FC_GameUtil::GetEnemyData(uint32 EnemyId)
{
	TArray<FC_EnemyTableRow*> EnemyTableRows = GetAllRows<FC_EnemyTableRow>(EC_DataTableType::Enemy);
	if (FC_EnemyTableRow** FoundRow = EnemyTableRows.FindByPredicate([EnemyId](const FC_EnemyTableRow* Row)
	{
		return Row->DataId == EnemyId;
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

ECollisionChannel FC_GameUtil::GetAttackCollisionChannel(uint32 DataId)
{
	if (DataId == 0)
	{
		return ECC_GameTraceChannel4;
	}

	return ECC_GameTraceChannel3;
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

FC_ExecTableRow* FC_GameUtil::GetExecData(uint32 ExecId)
{
	TArray<FC_ExecTableRow*> EnemyTableRows = GetAllRows<FC_ExecTableRow>(EC_DataTableType::Exec);
	if (FC_ExecTableRow** FoundRow = EnemyTableRows.FindByPredicate([ExecId](const FC_ExecTableRow* Row)
	{
		return Row->DataId == ExecId;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("ExecData is Invalid"));
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

FC_CrowdControlTableRow* FC_GameUtil::GetCrowdControlData(uint32 CrowdControlId)
{
	TArray<FC_CrowdControlTableRow*> EnemyTableRows = GetAllRows<FC_CrowdControlTableRow>(EC_DataTableType::CrowdControl);
	if (FC_CrowdControlTableRow** FoundRow = EnemyTableRows.FindByPredicate([CrowdControlId](const FC_CrowdControlTableRow* Row)
	{
		return Row->DataId == CrowdControlId;
	}))
	{
		return *FoundRow;
	}
	
	UE_LOG(LogProjectC, Error, TEXT("CrowdControlIData is Invalid"));
	return nullptr;
}

void FC_GameUtil::CameraShake()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GEngine->GetCurrentPlayWorld(), 0))
	{
		PlayerController->ClientStartCameraShake(UC_CameraShake::StaticClass());
	}
}

void FC_GameUtil::SpawnEffectAtLocation(UObject* WorldContextObj, UNiagaraSystem* NiagaraSystem, FVector Location, FRotator Rotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObj, NiagaraSystem, Location, Rotation);
}

void FC_GameUtil::SpawnEffectAtLocation(UObject* WorldContextObj, UParticleSystem* ParticleSystem, FVector Location, FRotator Rotation)
{
	UGameplayStatics::SpawnEmitterAtLocation(WorldContextObj, ParticleSystem, Location, Rotation);
}

UNiagaraComponent* FC_GameUtil::SpawnEffectAttached(UNiagaraSystem* NiagaraSystem, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bAutoDestroy)
{
	return UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bAutoDestroy);
}

TArray<UAnimMontage*> FC_GameUtil::GetComboAttackMontages(UC_PlayerDataAsset* PlayerDataAsset, EC_CharacterStanceType StanceType, bool bInSpecialAttack)
{
	TArray<FC_ComboAttackEntry>& ComboAttackMontageEntries = PlayerDataAsset->ComboAttackMontages;
	
	for (const FC_ComboAttackEntry& ComboEntry : ComboAttackMontageEntries)
	{
		if (ComboEntry.Key == FC_ComboKey(StanceType, bInSpecialAttack))
		{
			return ComboEntry.Data.AttackMontages;
		}
	}

	return TArray<UAnimMontage*>();
}

uint32 FC_GameUtil::GetSkillId(UC_PlayerDataAsset* PlayerDataAsset, EC_SkillSlotType SkillSlotType, EC_CharacterStanceType StanceType, bool bInSpecialAttack)
{
	TArray<FC_SkillEntry>& SkillIdEntries = PlayerDataAsset->SkillSlotDatas;
	
	for (const FC_SkillEntry& SkillEntry : SkillIdEntries)
	{
		if (SkillEntry.Key == FC_ComboKey(StanceType, bInSpecialAttack))
		{
			return *SkillEntry.Data.SkillIds.Find(SkillSlotType);
		}
	}

	return 0;
}

FVector FC_GameUtil::FindSurfacePos(ACharacter* Character, FVector& CurrentPos)
{
	UWorld* World = Character->GetWorld();
	check(World);

	FVector SweepStartPos = CurrentPos + FVector(0.f, 0.f, 200.f);
	FVector SweepEndPos = CurrentPos - FVector(0.f, 0.f, 200.f);

	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams QueryParams;
	
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	QueryParams.AddIgnoredActor(Character);

	float Radius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float Height = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(Radius, Height);
	
	FHitResult HitResult;
	World->SweepSingleByObjectType(HitResult, SweepStartPos, SweepEndPos, FQuat::Identity, ObjectQueryParams, CollisionShape, QueryParams);

	if (HitResult.bBlockingHit)
	{
		FVector TargetLocation = HitResult.ImpactPoint;
		FNavLocation ProjectedLocation;

		UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetCurrent(World);
		check(NavigationSystemV1);

		bool bValid = NavigationSystemV1->ProjectPointToNavigation(
			TargetLocation,
			ProjectedLocation,
			FVector(Radius, Radius, Height) // 주변 검사 범위
		);

		if (bValid)
		{
			return ProjectedLocation.Location + FVector(0.f, 0.f, Height);
		}
		else
		{
			return CurrentPos;
		}
	}
	
	return FVector::ZeroVector;
}

void FC_GameUtil::PlayHitStop(UObject* WorldObject, float Duration, float Dilation)
{
	UWorld* World = WorldObject->GetWorld();
	check(World);

	UGameplayStatics::SetGlobalTimeDilation(World, Dilation);

	// 0.2초 실제 시간 후 한 번만 실행
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([World](float)
		{
			UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
			return false;  // 다시 호출하지 않음
		}),
		Duration  // 실제 초 기준
	);
}
