// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectC/enums.h"
#include "C_BattleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_BattleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_BattleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartTrace(FName InTraceBoneName);
	void EndTrace();

	void SpawnEffect(EEffectType InEffectType, FVector InHitLocation);

public:
	//============================ Trace ============================
	bool bTracing = false;
	FName TraceBoneName;

	FVector PrevLocation = FVector::ZeroVector;

	float TraceInterval = 0.f;
	float TraceElapsedTime = 0.f;
	
	TArray<TWeakObjectPtr<AActor>> DamagedActor;
	//===============================================================
};
