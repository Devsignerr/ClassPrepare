// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_LockOnComponent.generated.h"


class UC_LockOnWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_LockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_LockOnComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	APawn* FindTarget();
	void LockTarget(APawn* InActor);
	void ClearTarget();

	UPROPERTY()
	TObjectPtr<APawn> Target = nullptr;

	float TargetDetectRadius = 0.f;
	float TargetDetectAngle = 0.f;
};
