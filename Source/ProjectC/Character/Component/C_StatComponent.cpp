// Fill out your copyright notice in the Description page of Project Settings.


#include "C_StatComponent.h"

#include "ProjectC/Character/C_CharacterBase.h"
#include "ProjectC/Utils/C_GameUtil.h"

UC_StatComponent::UC_StatComponent()
{
	bWantsInitializeComponent = true;
}

void UC_StatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AC_CharacterBase* CharacterBase = Cast<AC_CharacterBase>(GetOwner()))
	{
		if (FC_CharacterStatTableRow* StatRow = FC_GameUtil::GetCharacterStatData(CharacterBase->CharacterType))
			BaseStat = *StatRow;
	}
	
	ResetStat();
}

void UC_StatComponent::AddBaseStat(const FC_CharacterStatTableRow& InAddBaseStat)
{
	BaseStat = BaseStat + InAddBaseStat; 
	OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
}

void UC_StatComponent::SetBaseStat(const FC_CharacterStatTableRow& InBaseStat)
{
	BaseStat = InBaseStat;
	OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat()); 
}

void UC_StatComponent::SetModifierStat(const FC_CharacterStatTableRow& InModifierStat)
{
	ModifierStat = InModifierStat;
	OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
}

void UC_StatComponent::HealHp(float InHealAmount)
{
	CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0, GetTotalStat().MaxHp);
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

float UC_StatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpChanged.Broadcast(0, MaxHp);
	}

	return ActualDamage;
}

void UC_StatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

void UC_StatComponent::ResetStat()
{
	MaxHp = BaseStat.MaxHp;
	SetHp(MaxHp);
}

