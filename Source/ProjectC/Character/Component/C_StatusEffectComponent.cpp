// Fill out your copyright notice in the Description page of Project Settings.


#include "C_StatusEffectComponent.h"

UC_StatusEffectComponent::UC_StatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_StatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UC_StatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
