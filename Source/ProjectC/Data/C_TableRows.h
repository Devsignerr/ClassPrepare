
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "Engine/DataTable.h"
#include "ProjectC/enums.h"
#include "NiagaraSystem.h"
#include "ProjectC/Character/Component/C_SkillComponent.h"
#include "C_TableRows.generated.h"

USTRUCT(BlueprintType)
struct FC_ExecData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float DelayTime = 0.f;

	UPROPERTY(EditAnywhere)
	uint32 ExecDataId = 0;
};

USTRUCT(BlueprintType)
struct FC_CharacterStatTableRow : public FTableRowBase
{
	GENERATED_BODY()

	FC_CharacterStatTableRow() : MaxHp(0.0f), Attack(0.0f){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EC_CharacterType CharacterType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	FC_CharacterStatTableRow operator+(const FC_CharacterStatTableRow& Other) const
	{
		FC_CharacterStatTableRow Result;
		
		Result.MaxHp = this->MaxHp + Other.MaxHp;
		Result.Attack = this->Attack + Other.Attack;

		return Result;
	}
};


USTRUCT(BlueprintType)
struct FC_EnemyTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EC_CharacterType EnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBlackboardData> BlackBoard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed_Walk = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed_Run = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRadius = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PeripheralVisionAngleDegrees = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoSuccessRangeFromLastSeenLocation = 0.f;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AttackAnim = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Left90TurnAnim = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Left180TurnAnim = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Right90TurnAnim = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Right180TurnAnim = nullptr;
};

USTRUCT(BlueprintType)
struct FC_WeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 WeaponId = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TraceStartSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TraceEndSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RelativePos = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RelativeRot = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FC_SkillTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	uint32 DataId = 0;

	UPROPERTY(EditAnywhere)
	float CoolTime = 0.f;

	UPROPERTY(EditAnywhere)
	float SkillRange = 0.f;
	
	UPROPERTY(EditAnywhere)
	EC_SkillTargetingType SkillTargetingType = EC_SkillTargetingType::None;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* SkillActivateFx;
	
	UPROPERTY(EditAnywhere)
	TArray<FC_ExecData> ExecDatas;
};

USTRUCT(BlueprintType)
struct FC_ExecTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	uint32 DataId = 0;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> SkillAnim;
	
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ExecFX_Niagara_Start = nullptr;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ExecFX_Cascade_Start = nullptr;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ExecFX_Niagara_End = nullptr;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ExecFX_Cascade_End = nullptr;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* HitFX_Niagara = nullptr;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* HitFX_Cascade = nullptr;

	UPROPERTY(EditAnywhere)
	float HitEffectScale = 1.f;
	
	UPROPERTY(EditAnywhere)
	float Duration = 0.f;

	UPROPERTY(EditAnywhere)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere)
	EC_ExecType ExecType = EC_ExecType::None;

	UPROPERTY(EditAnywhere)
	EC_ExecCollisionType ExecCollisionType = EC_ExecCollisionType::None;

	UPROPERTY(EditAnywhere)
	UCurveFloat* ExecCurve = nullptr;

	UPROPERTY(EditAnywhere)
	float ExecProperty_0 = 0;
	
	UPROPERTY(EditAnywhere)
	float ExecProperty_1 = 0;

	UPROPERTY(EditAnywhere)
	float ExecProperty_2 = 0;

	UPROPERTY(EditAnywhere)
	float ExecCollisionProperty_0 = 0;

	UPROPERTY(EditAnywhere)
	float ExecCollisionProperty_1 = 0;

	UPROPERTY(EditAnywhere)
	float ExecCollisionProperty_2 = 0;

	UPROPERTY(EditAnywhere)
	float CrowdControlId = INDEX_NONE;

	UPROPERTY(EditAnywhere)
	bool bAttachFx = false;

	UPROPERTY(EditAnywhere)
	FName SkillPosBoneName = NAME_None;

	UPROPERTY(EditAnywhere)
	bool bPlayCameraShake = true;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> HitMaterial = nullptr;
};

USTRUCT(BlueprintType)
struct FC_SkillObjectTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	uint32 DataId = 0;

	UPROPERTY(EditAnywhere)
	float Damage = 0;

	UPROPERTY(EditAnywhere)
	EC_SkillObjectType SkillObjectType = EC_SkillObjectType::None;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SkillObjectActor;
};

USTRUCT(BlueprintType)
struct FC_CrowdControlTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	uint32 DataId = 0;

	UPROPERTY(EditAnywhere)
	EC_CrowdControlType CrowdControlType = EC_CrowdControlType::None;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> CrowdControlFX = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> MaterialInterface = nullptr;

	UPROPERTY(EditAnywhere)
	float Duration = 0.f;

	UPROPERTY(EditAnywhere)
	float Property_0 = 0.f;

	UPROPERTY(EditAnywhere)
	float Property_1 = 0.f;

	UPROPERTY(EditAnywhere)
	float Property_2 = 0.f;
};