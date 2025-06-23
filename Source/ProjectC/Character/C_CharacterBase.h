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
	virtual void AttackTraceWithWeapon(bool bStart) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	void ApplyStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat);

	virtual void SetupCharacterWidget(class UC_UserWidget* InUserWidget) override;
	virtual void SetupLockOnWidget(UC_UserWidget* InUserWidget) override;
	virtual void OnLocked(bool bLocked) override;

	virtual bool HasWeapon() override;

	virtual UStaticMeshComponent* GetStaticMeshComponent() override { return WeaponStaticComponent; }
	virtual UC_CrowdControlComponent* GetCrowdControlComponent() override { return CrowdControlComponent; }
	virtual TPair<FName, FName> GetWeaponTraceNames() override;
	virtual UC_CharacterDataAsset* GetCharacterDataAsset() override { return CharacterData; }

	UFUNCTION()
	virtual void OnStartCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer);

	UFUNCTION()
	virtual void OnEndCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer);
	
public:
	UPROPERTY(EditAnywhere) 
	EC_CharacterType CharacterType;
	
	FOnCharacterLocked OnCharacterLocked;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_BattleComponent> BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_CrowdControlComponent> CrowdControlComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_StatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_WidgetComponent> WidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponStaticComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_CharacterDataAsset> CharacterData;
};

