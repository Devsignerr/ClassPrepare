
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

void UC_SkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Tick_PlaySkill(DeltaTime);
}

void UC_SkillComponent::Tick_PlaySkill(float DeltaTime)
{
	TArray<FC_SkillInfo> SkillToCoolDown;
	
	for (FC_SkillInfo& SkillInfo : CurrentPlayingSkillInfos)
	{
		FC_SkillTableRow* SkillTableRow = FC_GameUtil::GetSkillData(SkillInfo.SkillDataId);
		check(SkillTableRow);

		SkillInfo.ElapsedTime += DeltaTime;
		if (SkillInfo.ElapsedTime > SkillInfo.LifeTime)
		{
			SkillToCoolDown.Add(SkillInfo);
			continue;
		}

		if (SkillTableRow->SkillTargetingType == EC_SkillTargetingType::NoneTarget)
		{
			ProcessNoneTargetSkill(DeltaTime, SkillInfo);
		}
	}

	for (FC_SkillInfo& SkillInfo : SkillToCoolDown)
	{
		CoolDownSkillInfos.Add(SkillInfo);
	}

	CurrentPlayingSkillInfos.RemoveAll(
	[&](const FC_SkillInfo& Skill)
	{
		return Skill.ElapsedTime > Skill.LifeTime;
	});
}

void UC_SkillComponent::InitSkillInfo(uint32 SkillId, TArray<TWeakObjectPtr<AActor>> Targets, FC_SkillInfo& SkillInfo)
{
	SkillInfo.SkillDataId = SkillId;
	SkillInfo.Targets = Targets;
	SkillInfo.SkillStartPos = OwnerCharacter->GetActorLocation();
	SkillInfo.SkillStartRot = OwnerCharacter->GetActorRotation();

	float SkillLifeTime = 0.f;
	TArray<FC_ExecInfo> ExecInfos;
	ExecInfos.AddDefaulted(FC_GameUtil::GetSkillData(SkillId)->ExecDatas.Num());

	CalcSkillTime(SkillId, SkillLifeTime, ExecInfos);
		
	SkillInfo.LifeTime = SkillLifeTime;
	SkillInfo.ElapsedTime = 0.f;
	SkillInfo.ExecInfos = ExecInfos;
}

void UC_SkillComponent::RequestPlaySkill(uint32 SkillId)
{
	if (!CanPlaySkill(SkillId))
		return;

	TArray<TWeakObjectPtr<AActor>> Targets;
	FindTargets(SkillId, Targets);

	if (!Targets.IsEmpty() || FC_GameUtil::GetSkillData(SkillId)->SkillTargetingType == EC_SkillTargetingType::NoneTarget)
	{
		FC_SkillInfo SkillInfo;
		InitSkillInfo(SkillId, Targets, SkillInfo); 

		PlaySkill(SkillInfo);
	}
}

void UC_SkillComponent::FindTargets(uint32 SkillId, TArray<TWeakObjectPtr<AActor>>& Targets)
{
	TArray<AActor*> Enemys;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Enemys);

	const float SkillRange = FC_GameUtil::GetSkillData(SkillId)->SkillRange;
	const FVector OwnerCharacterPos = OwnerCharacter->GetActorLocation();

	for (AActor* Enemy : Enemys)
	{
		if (Enemy != GetOwner())
		{
			if (FVector::Dist(OwnerCharacterPos, Enemy->GetActorLocation()) <= SkillRange)
			{
				Targets.Add(Enemy);
			}
		}
	}
}

bool UC_SkillComponent::CanPlaySkill(uint32 SkillId)
{
	return true;
}

void UC_SkillComponent::PlaySkill(FC_SkillInfo& SkillInfo)
{
	CurrentPlayingSkillInfos.Add(SkillInfo);
}

void UC_SkillComponent::CalcSkillTime(uint32 SKillId, float& SkillLifeTime, TArray<FC_ExecInfo>& ExecInfos)
{
	for (int32 i = 0; i < ExecInfos.Num(); ++i)
	{
		FC_ExecData& ExecData = FC_GameUtil::GetSkillData(SKillId)->ExecDatas[i];
		FC_ExecInfo& ExecInfo = ExecInfos[i];
		ExecInfo.ExecData = &ExecData;
		ExecInfo.AnimStartTime = SkillLifeTime;
		ExecInfo.ExecStartTime = SkillLifeTime + ExecData.DelayTime;
		ExecInfo.EndTime = ExecInfo.ExecStartTime + FC_GameUtil::GetExecData(ExecData.ExecDataId)->Duration;

		SkillLifeTime += ExecData.DelayTime + FC_GameUtil::GetExecData(ExecData.ExecDataId)->Duration;
	}
}

void UC_SkillComponent::ProcessNoneTargetSkill(float DeltaTime, FC_SkillInfo& SkillInfo)
{
	float ElapsedTime = SkillInfo.ElapsedTime; 
	
	for (FC_ExecInfo& ExecInfo : SkillInfo.ExecInfos)
	{
		if (ElapsedTime > ExecInfo.AnimStartTime && !ExecInfo.bAnimStarted)
		{
			FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
			check(ExecTableRow);

			UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
			check(AnimInstance);

			ExecInfo.bAnimStarted = true;

			AnimInstance->StopAllMontages(0.1f);
			AnimInstance->Montage_Play(ExecTableRow->SkillAnim);
		}

		if (ElapsedTime > ExecInfo.ExecStartTime && ElapsedTime < ExecInfo.EndTime)
		{
			ExecInfo.bExecStarted = true;
			ProcessNoneTargetExec(DeltaTime, ExecInfo, SkillInfo.SkillStartPos, SkillInfo.SkillStartRot);
		}
	}
}

void UC_SkillComponent::ProcessNoneTargetExec(float DeltaTime, FC_ExecInfo& ExecInfo, FVector StartPos, FRotator StartRot)
{
	FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
	check(ExecTableRow);
	
	if (ExecTableRow->ExecType == EC_ExecType::Dash)
	{
		float DashRange = ExecTableRow->ExecProperty_0;
		float Duration = ExecTableRow->Duration;

		float Speed = DashRange / Duration;

		FVector CurrentPos = OwnerCharacter->GetActorLocation();
		FVector NewPos = CurrentPos + StartRot.Vector() * Speed * DeltaTime;

		FHitResult Result;
		OwnerCharacter->SetActorLocation(NewPos, true, &Result);
		if (Result.bBlockingHit)
		{
			OwnerCharacter->SetActorLocation(CurrentPos);
		}
	}
}
