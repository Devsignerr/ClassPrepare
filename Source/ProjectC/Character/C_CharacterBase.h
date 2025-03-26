// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Component/C_BattleComponent.h"
#include "Component/C_StatComponent.h"
#include "Component/C_WidgetComponent.h"
#include "..\Data\C_TableRows.h"
#include "ProjectC/Interface/C_AnimationAttackInterface.h"
#include "ProjectC/Interface/C_CharacterWidgetInterface.h"
#include "C_CharacterBase.generated.h"


UCLASS(config=Game)
class PROJECTC_API AC_CharacterBase : public ACharacter , public IC_AnimationAttackInterface , public IC_CharacterWidgetInterface
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
	
public:
	void ApplyStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat);
	
public:
	UPROPERTY(EditAnywhere) 
	EC_CharacterType CharacterType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_BattleComponent> BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_StatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UC_WidgetComponent> WidgetComponent;
};

