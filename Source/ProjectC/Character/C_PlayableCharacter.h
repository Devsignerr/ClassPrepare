// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


//Unreal Engine에서 자주 쓰이는 핵심 타입, 매크로, 기능들만 최소한으로 모아놓은 헤더파일이에요.
//ObjectMacros.h	UCLASS, UPROPERTY, UFUNCTION 같은 매크로
//EngineMinimal.h	게임 엔진 관련 최소 헤더
//Math 관련 클래스	FVector, FRotator, FTransform, FQuat 등 수학 클래스
//기본 컨테이너	TArray, TMap, TSet, FString, FName 등
//스마트 포인터	TSharedPtr, TWeakObjectPtr, TUniquePtr 등
//로깅 시스템	UE_LOG() 사용 가능
//assert 및 디버깅	check(), ensure(), UE_DEBUG_BREAK() 등

#include "CoreMinimal.h"
#include "C_CharacterBase.h"
#include "InputActionValue.h"
#include "ProjectC/Interface/C_CharacterHUDInterface.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"
#include "C_PlayableCharacter.generated.h"


class UC_PlayerDataAsset;
class UC_InputDataAsset;
class UC_ActionComponent;
class UC_LockOnComponent;
class UAIPerceptionStimuliSourceComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS(config=Game)
class AC_PlayableCharacter : public AC_CharacterBase , public IC_CharacterHUDInterface , public IGenericTeamAgentInterface , public IC_PlayerCharacterInterface
{
	GENERATED_BODY()

public:
	AC_PlayableCharacter();
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void Guard(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void Roll(const FInputActionValue& Value);
	void LockOn(const FInputActionValue& Value);

public:
	virtual void SetupHUDWidget(class UC_HUDWidget* InHUDWidget) override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

public:
	virtual UStaticMeshComponent* GetWeaponStaticMeshComponent() const override { return WeaponStaticComponent; }
	virtual UC_ActionComponent* GetActionComponent() const override { return ActionComponent; }
	virtual UC_LockOnComponent* GetLockOnComponent() const override { return LockOnComponent; }
	virtual UC_BattleComponent* GetBattleComponent() const override { return BattleComponent; }
	virtual UC_PlayerDataAsset* GetPlayerData() const override { return PlayerData; }
public:
	//============= Component =========================================================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimulusSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_ActionComponent> ActionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_LockOnComponent> LockOnComponent;

	//============= Input ===========================================================================
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_InputDataAsset> InputData;

	//===============================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_PlayerDataAsset> PlayerData;
	
	UPROPERTY()
	FGenericTeamId GenericTeamId = 0;
};

