// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PatrolRoute.h"

#include "Components/SplineComponent.h"

// Sets default values
AC_PatrolRoute::AC_PatrolRoute()
{
	PatrolSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SetRootComponent(PatrolSplineComponent);
}

// Called when the game starts or when spawned
void AC_PatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	
}

void AC_PatrolRoute::IncrementIndex()
{
	check(PatrolSplineComponent);
	const int SplinePoint = PatrolSplineComponent->GetNumberOfSplinePoints();
	
	CurrentPatrolIndex = CurrentPatrolIndex + Direction;
	
	if (CurrentPatrolIndex == SplinePoint - 1)
	{
		Direction = -1;
	}
	else if (CurrentPatrolIndex == 0)
	{
		Direction = 1;
	}
}

FVector AC_PatrolRoute::GetLocationAtSplinePoint()
{
	check(PatrolSplineComponent);
	return PatrolSplineComponent->GetLocationAtSplinePoint(CurrentPatrolIndex, ESplineCoordinateSpace::World);
}

