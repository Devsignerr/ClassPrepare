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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void LockOn();

	APawn* FindTarget();
	void LockTarget(APawn* InActor);
	void ClearTarget();
	APawn* GetLockTarget() const { return LockedTarget.Get(); }

	virtual void SetLockOnMode(bool bEnable);
	bool IsLockOnMode() const { return bLockOnMode; }

private:
	TWeakObjectPtr<APawn> LockedTarget = nullptr;

	float TargetDetectRadius = 0.f;
	float TargetDetectAngle = 0.f;

public:
	UPROPERTY(BlueprintReadOnly)
	bool bLockOnMode = false;
};
