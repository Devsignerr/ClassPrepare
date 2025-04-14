// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectC/enums.h"
#include "C_ActionComponent.generated.h"

USTRUCT(BlueprintType)
struct FC_LockData
{
	GENERATED_BODY()

	UPROPERTY()
	EC_LockCauseType LockCauseType = EC_LockCauseType::None;

	UPROPERTY()
	EC_ActionType LockType = EC_ActionType::None;

	FC_LockData(){}
	FC_LockData(EC_LockCauseType InCause, EC_ActionType InType) : LockCauseType(InCause), LockType(InType) {}
};

//Attack, Roll, Guard, Run 등 특수액션 상태관리 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTC_API UC_ActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_ActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Move(FVector2D MovementVector);
	void Jump(bool IsPressed);
	void Attack(bool IsPressed);
	void Guard(bool bPressed);
	void Run(bool bPressed);
	void Roll(bool bPressed);

	bool CanAction(EC_ActionType InActionType);

	bool IsLocked(EC_ActionType InLockType);
	void AddLock(EC_LockCauseType InLockCauseType, EC_ActionType InLockType);
	void ReleaseLock(EC_LockCauseType InLockCauseType, EC_ActionType InLockType);
	void ForceReleaseLock(EC_LockCauseType InLockCauseType);
	void ResetAllLock();

	void ProcessFreeMove();
	void ProcessLockOnMove();
	
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION(BlueprintCallable)
	void ComboAttackSave();

	UFUNCTION(BlueprintCallable)
	void ResetCombo();

	void RotateToControlRotation();

	void OnGuardSuccess(AActor* DamageCauser);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SaveAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsGuarding = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsRolling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AttackCount = 0;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
	TArray<FC_LockData> LockData;

	UPROPERTY(BlueprintReadOnly)
	FVector2D InputVector = FVector2D::ZeroVector;
};
