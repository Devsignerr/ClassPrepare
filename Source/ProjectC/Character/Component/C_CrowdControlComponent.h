// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectC/enums.h"
#include "C_CrowdControlComponent.generated.h"

class UNiagaraComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FC_OnStartCCDelegate, EC_CrowdControlType, CrowdControlType, AActor*, Causer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FC_OnEndCCDelegate, EC_CrowdControlType, CrowdControlType, AActor*, Causer);

struct FC_CrowdControlInfo
{
	uint32 CrowdControlDataId = -1;
	EC_CrowdControlType CrowdControlType = EC_CrowdControlType::None;
	float LifeTime = 0.f;
	float ElapsedTime = 0.f;

	FVector CauserPos = FVector::ZeroVector;
	FRotator CauserRot = FRotator::ZeroRotator;

	FVector StartPos = FVector::ZeroVector;
	FRotator StartRot = FRotator::ZeroRotator;

	TObjectPtr<UNiagaraComponent> SpawnedFX = nullptr;
	TWeakObjectPtr<AActor> Causer = nullptr;

	bool bValid = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_CrowdControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_CrowdControlComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Tick_PlayCrowdControl(float DeltaTime);

	void ProcessCC();
	
	void RequestPlayCC(uint32 CrowdControlId, AActor* Causer);
	bool CanPlayCC(FC_CrowdControlInfo& Info);
	void PlayCC(FC_CrowdControlInfo& Info);
	void StopCC();

	void OnStartCC();
	void OnStopCC();

	void PlayFX(FC_CrowdControlInfo& Info);
	void StopFX();

	bool IsCrowdControlled();

public:
	FC_OnStartCCDelegate OnStartCCDelegate;
	FC_OnEndCCDelegate OnEndCCDelegate;

	FC_CrowdControlInfo CrowdControlInfo;
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
};
