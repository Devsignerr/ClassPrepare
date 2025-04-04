// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Component/C_BattleComponent.h"
#include "Component/C_StatComponent.h"
#include "Component/C_WidgetComponent.h"
#include "..\Data\C_TableRows.h"
#include "ProjectC/Interface/C_AnimationAttackInterface.h"
#include "ProjectC/Interface/C_CharacterWidgetInterface.h"
#include "C_CharacterBase.generated.h"


UCLASS(config=Game)
class PROJECTC_API AC_CharacterBase : public ACharacter , public IC_AnimationAttackInterface , public IC_CharacterWidgetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AC_CharacterBase();
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void AttackTrace(bool bStart, FName TraceBoneName) override;
	virtual void SetupCharacterWidget(class UC_UserWidget* InUserWidget) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	void ApplyStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat);

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	
public:
	UPROPERTY(EditAnywhere) 
	EC_CharacterType CharacterType;

	UPROPERTY()
	FGenericTeamId GenericTeamId = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_BattleComponent> BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_StatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_WidgetComponent> WidgetComponent;
};

