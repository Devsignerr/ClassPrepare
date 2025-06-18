// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SkillComponent.generated.h"

struct FC_ExecData;

struct FC_ExecInfo
{
	FC_ExecData* ExecData = nullptr;

	bool bAnimStarted = false;
	bool bExecStarted = false;
	bool bExecCollisionSpawned = false;
	bool bExecFinished = false;

	float ElapsedTime = 0.f;

	float AnimStartTime = 0.f;
	float ExecStartTime = 0.f;
	float EndTime = 0.f;

	uint32 ExecSequence = 0;

	FVector ExecStartPos = FVector::ZeroVector;
	FRotator ExecStartRot = FRotator::ZeroRotator;
};

struct FC_SkillInfo
{
	uint32 SkillDataId = 0;

	float LifeTime = 0.f;
	float ElapsedTime = 0.f;
	
	TArray<TWeakObjectPtr<AActor>> Targets;
	
	FVector SkillStartPos = FVector::ZeroVector;
	FRotator SkillStartRot = FRotator::ZeroRotator;

	TArray<FC_ExecInfo> ExecInfos;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_SkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_SkillComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void Tick_PlaySkill(float DeltaTime);
	
public:
	void RequestPlaySkill(uint32 SkillId);
	void FindTargets(uint32 SkillId, TArray<TWeakObjectPtr<AActor>>& Targets);
	bool CanPlaySkill(uint32 SkillId);
	void PlaySkill(FC_SkillInfo& SkillInfo);

	void InitSkillInfo(uint32 SkillId, TArray<TWeakObjectPtr<AActor>> Targets, FC_SkillInfo& SkillInfo);
	void CalcSkillTime(uint32 SKillId, float& SkillLifeTime, TArray<FC_ExecInfo>& ExecInfos);

	void ProcessSkill(float DeltaTime, FC_SkillInfo& SkillInfo);
	
	void ProcessNoneTargetExec(float DeltaTime, FC_ExecInfo& ExecInfo, FVector StartPos, FRotator StartRot);
	void ProcessChainAttackExec(float DeltaTime, FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo, FVector StartPos, FRotator StartRot);
	void ProcessMultipleExec(float DeltaTime, FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo, FVector StartPos, FRotator StartRot);

	void SpawnExecCollision(FC_ExecInfo& ExecInfo, FCollisionShape CollisionShape, FVector Pos, FRotator Rot);

	void OnStartExec(FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo);
	void OnEndExec(FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo);
public:
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
	TArray<FC_SkillInfo> CurrentPlayingSkillInfos;
	TArray<FC_SkillInfo> CoolDownSkillInfos;
	
	TArray<uint32> CoolDownSkillId;
};
