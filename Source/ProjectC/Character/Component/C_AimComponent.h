// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectC/enums.h"
#include "C_AimComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_AimComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_AimComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void CalcAimOffset(float DeltaTime);

	void SwitchCamera(EC_CameraType CameraType);

	UPROPERTY(BlueprintReadOnly)
	FRotator AimOffsetRotation = FRotator::ZeroRotator;

	EC_CameraType CurrentCameraType = EC_CameraType::Normal;
	bool bCameraBlending = false;
	
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
};
