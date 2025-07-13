// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectC/enums.h"
#include "ProjectC/Data/C_CharacterDataAsset.h"
#include "C_BattleComponent.generated.h"


struct FC_WeaponTableRow;

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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartTraceWithWeapon(bool bRight);
	void StartTrace(FName InTraceStartBoneName, FName InTraceEndBoneName);
	void EndTrace();

	bool CanSwapWeapon();
	void SwapWeapon();
	
	void EquipWeapon(uint8 InWeaponId, bool bRightHand);
	void UnEquipWeapon();
	bool HasWeapon();

	void FireProjectile();

public:
	//============================ Trace ============================
	bool bTracing = false;
	bool bTraceRightWeapon = false;
	
	FName TraceStartBoneName;
	FName TraceEndBoneName;

	FVector PrevStartBoneLocation = FVector::ZeroVector;
	FVector PrevEndBoneLocation = FVector::ZeroVector;
	
	float TraceInterval = 0.f;
	float TraceElapsedTime = 0.f;
	
	TArray<TWeakObjectPtr<AActor>> DamagedActor;
	
	//========================= Weapon ==============================
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

	FC_WeaponTableRow* Weapon_L_TableRow = nullptr;
	FC_WeaponTableRow* Weapon_R_TableRow = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	EC_CharacterStanceType CharacterStanceType = EC_CharacterStanceType::Sword;

	int32 CurWeaponIdx = 0;
	TArray<FC_WeaponData> Weapons; 
};
