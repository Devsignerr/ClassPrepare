// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ProjectC/enums.h"
#include "C_AIController.generated.h"

class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
struct FC_EnemyTableRow;
class UC_AIPerceptionComponent;

UCLASS()
class PROJECTC_API AC_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AC_AIController();
	
	void RunAI();
	void StopAI();
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	FC_EnemyTableRow* GetEnemyData();

	UFUNCTION()
	void OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors);
	
	void SetupSenseConfig();
	FAIStimulus GetAIStimulus(AActor* Actor, EC_AISenseType AIPerceptionSense);

	void HandleSensedSight(AActor* InActor);
	void HandleSensedHearing(AActor* InActor, FVector InLocation);
	void HandleSensedDamage(AActor* InActor);
	
	void HandleLoseTarget(AActor* InActor);
protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;
	
	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;
	
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightSense = nullptr;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingSense = nullptr;

	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageSense = nullptr;
};
