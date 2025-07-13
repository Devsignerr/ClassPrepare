// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Component/C_BattleComponent.h"
#include "Component/C_StatComponent.h"
#include "Component/C_WidgetComponent.h"
#include "..\Data\C_TableRows.h"
#include "..\Interface\C_CharacterInterface.h"
#include "ProjectC/Interface/C_CharacterWidgetInterface.h"
#include "C_CharacterBase.generated.h"

class UC_CharacterDataAsset;
class UC_CrowdControlComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterLocked, bool, bLocked);

UCLASS(config=Game)
class PROJECTC_API AC_CharacterBase : public ACharacter , public IC_CharacterInterface , public IC_CharacterWidgetInterface
{
	GENERATED_BODY()

public:
	AC_CharacterBase();
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	virtual void AttackTrace(bool bStart, FName TraceStartBoneName, FName TraceEndBoneName) override;
	virtual void AttackTraceWithWeapon(bool bStart, bool bRight) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	void ApplyStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat);

	virtual void SetupCharacterWidget(class UC_UserWidget* InUserWidget) override;
	virtual void SetupLockOnWidget(UC_UserWidget* InUserWidget) override;
	virtual void OnLocked(bool bLocked) override;

	virtual bool HasWeapon() override;

	virtual void OnDead();
	virtual bool IsDead() override;
	
	UFUNCTION()
	virtual void OnLand(const FHitResult& Result);

	virtual UStaticMeshComponent* GetWeapon_L_StaticMeshComponent() override { return Weapon_L_StaticComponent; }
	virtual UStaticMeshComponent* GetWeapon_R_StaticMeshComponent() override { return Weapon_R_StaticComponent; }
	virtual UC_CrowdControlComponent* GetCrowdControlComponent() override { return CrowdControlComponent; }
	virtual UC_BattleComponent* GetBattleComponent() const override { return BattleComponent; }
	virtual UC_SkillComponent* GetSkillComponent() const override { return SkillComponent; }
	virtual TPair<FName, FName> GetWeaponTraceNames(bool bRight) override;
	virtual UC_CharacterDataAsset* GetCharacterDataAsset() override { return CharacterData; }

	UFUNCTION()
	virtual void OnStartCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer);

	UFUNCTION()
	virtual void OnEndCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer);

	virtual FC_OnStartSkillDelegate& GetOnStartSkillDelegate() override;
	virtual FC_OnEndSkillDelegate& GetOnEndSkillDelegate() override;
	
	virtual FC_OnLandDelegate* GetOnLandedDelegate() override { return &OnLandedDelegate; }
	
	UFUNCTION()
	virtual void OnStartSkill(uint32 SkillId);

	UFUNCTION()
	virtual void OnEndSkill(uint32 SkillId);
	
public:
	FC_OnLandDelegate OnLandedDelegate;
	
	FOnCharacterLocked OnCharacterLocked;

	UPROPERTY(EditAnywhere) 
	uint32 CharacterDataId = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_BattleComponent> BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_CrowdControlComponent> CrowdControlComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_StatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_WidgetComponent> WidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Weapon_L_StaticComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Weapon_R_StaticComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_SkillComponent> SkillComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_CharacterDataAsset> CharacterData;
};

