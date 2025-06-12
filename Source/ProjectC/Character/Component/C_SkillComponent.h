// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_SkillComponent.generated.h"

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

public:
	void RequestPlaySkill(int32 SkillId);
	bool CanPlaySkill(int32 SkillId);
	void PlaySkill(int32 SkillId);

	void ProcessComboAttack();
	void ProcessProjectile();
	void ProcessActiveSkill();

public:
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
	
};
