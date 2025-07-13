// Fill out your copyright notice in the Description page of Project Settings.


#include "C_StatComponent.h"

#include "C_ActionComponent.h"
#include "ProjectC/Character/C_CharacterBase.h"
#include "ProjectC/Data/C_CharacterDataAsset.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"
#include "ProjectC/Utils/C_GameUtil.h"

UC_StatComponent::UC_StatComponent()
{
	bWantsInitializeComponent = true;
}

void UC_StatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UC_StatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AC_CharacterBase* CharacterBase = Cast<AC_CharacterBase>(GetOwner()))
	{
		if (FC_CharacterStatTableRow* StatRow = FC_GameUtil::GetCharacterStatData(CharacterBase->CharacterDataId))
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

float UC_StatComponent::ApplyDamage(float InDamage, AActor* DamageCauser)
{
	IC_CharacterInterface* OwnerCharacterInterface = Cast<IC_CharacterInterface>(GetOwner());
	check(OwnerCharacterInterface);
	
	IC_CharacterInterface* CauserCharacterInterface = Cast<IC_CharacterInterface>(DamageCauser);
	check(CauserCharacterInterface);

	UC_CharacterDataAsset* OwnerDataAsset = OwnerCharacterInterface->GetCharacterDataAsset();
	check(OwnerDataAsset);

	UC_CharacterDataAsset* CauserDataAsset = CauserCharacterInterface->GetCharacterDataAsset();
	check(CauserDataAsset);

	if (const IC_PlayerCharacterInterface* Interface = Cast<IC_PlayerCharacterInterface>(GetOwner()))
	{
		IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
		
		UC_ActionComponent* ActionComponent = Interface->GetActionComponent();
		check(ActionComponent);

		UC_BattleComponent* BattleComponent = CharacterInterface->GetBattleComponent();
		check(ActionComponent);
		
		if (ActionComponent->IsRolling)
			return 0.f;

		if (ActionComponent->IsInSpecialAction && BattleComponent->CharacterStanceType == EC_CharacterStanceType::Sword)
		{
			ActionComponent->OnGuardSuccess(DamageCauser);
			FC_GameUtil::SpawnEffectAtLocation(GetWorld(), OwnerDataAsset->GuardFX, OwnerCharacter->GetActorLocation(), FRotator::ZeroRotator);
			
			return 0.f;
		}
	}

	FC_GameUtil::SpawnEffectAtLocation(GetWorld(), CauserDataAsset->HitFX, OwnerCharacter->GetActorLocation(), FRotator::ZeroRotator);
	
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnDead.Broadcast();
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

