
#include "C_SkillComponent.h"

#include "GameFramework/Character.h"
#include "ProjectC/Data/C_TableRows.h"
#include "ProjectC/Utils/C_GameUtil.h"

UC_SkillComponent::UC_SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_SkillComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
}

void UC_SkillComponent::RequestPlaySkill(int32 SkillId)
{
	if (!CanPlaySkill(SkillId))
		return;
	
	PlaySkill(SkillId);
}

bool UC_SkillComponent::CanPlaySkill(int32 SkillId)
{
	return true;
}

void UC_SkillComponent::PlaySkill(int32 SkillId)
{
	
}

void UC_SkillComponent::ProcessComboAttack()
{
}

void UC_SkillComponent::ProcessProjectile()
{
}

void UC_SkillComponent::ProcessActiveSkill()
{
}

void UC_SkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}