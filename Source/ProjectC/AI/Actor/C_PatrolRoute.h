// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_PatrolRoute.generated.h"

class USplineComponent;

UCLASS()
class PROJECTC_API AC_PatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_PatrolRoute();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void IncrementIndex();
	FVector GetLocationAtSplinePoint();

protected:
	int CurrentPatrolIndex = 0;
	int Direction = 0;
	
	UPROPERTY(EditAnywhere)
	USplineComponent* PatrolSplineComponent = nullptr;
};
