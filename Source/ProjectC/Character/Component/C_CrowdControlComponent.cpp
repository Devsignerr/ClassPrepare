#include "C_CrowdControlComponent.h"

#include "AIController.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectC/Data/C_CharacterDataAsset.h"
#include "ProjectC/Data/C_TableRows.h"
#include "ProjectC/Interface/C_CharacterInterface.h"
#include "ProjectC/Utils/C_GameUtil.h"

UC_CrowdControlComponent::UC_CrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_CrowdControlComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
}

void UC_CrowdControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Tick_PlayCrowdControl(DeltaTime);
}

void UC_CrowdControlComponent::Tick_PlayCrowdControl(float DeltaTime)
{
	TArray<FC_CrowdControlInfo> InfoToRemove;
	
	for (FC_CrowdControlInfo& CrowdControlInfo : CrowdControlInfos)
	{
		CrowdControlInfo.ElapsedTime += DeltaTime;
		
		ProcessCC(CrowdControlInfo);

		if (CrowdControlInfo.ElapsedTime > CrowdControlInfo.LifeTime)
		{
			InfoToRemove.Add(CrowdControlInfo);
		}
	}

	for (FC_CrowdControlInfo& CrowdControlInfo : InfoToRemove)
	{
		StopCC(CrowdControlInfo);
	}

	CrowdControlInfos.RemoveAll(
	[&](const FC_CrowdControlInfo& CrowdControl)
	{
		return CrowdControl.ElapsedTime > CrowdControl.LifeTime;
	});
}

void UC_CrowdControlComponent::ProcessCC(FC_CrowdControlInfo& CrowdControlInfo)
{
	
}

void UC_CrowdControlComponent::RequestPlayCC(uint32 CrowdControlId, AActor* Causer)
{
	FC_CrowdControlTableRow* CrowdControlTableRow = FC_GameUtil::GetCrowdControlData(CrowdControlId);
	check(CrowdControlTableRow);
	
	FC_CrowdControlInfo CrowdControlInfo;
	CrowdControlInfo.LifeTime = CrowdControlTableRow->Duration;
	CrowdControlInfo.ElapsedTime = 0.f;
	CrowdControlInfo.CrowdControlType = CrowdControlTableRow->CrowdControlType;
	CrowdControlInfo.CrowdControlDataId = CrowdControlTableRow->DataId;
	CrowdControlInfo.Causer = Causer;

	CrowdControlInfo.CauserPos = Causer->GetActorLocation();
	CrowdControlInfo.CauserRot = Causer->GetActorRotation();
	CrowdControlInfo.StartPos = OwnerCharacter->GetActorLocation();
	CrowdControlInfo.StartRot = OwnerCharacter->GetActorRotation();
	
	if (!CanPlayCC(CrowdControlInfo))
		return;

	PlayCC(CrowdControlInfo);	
}

bool UC_CrowdControlComponent::CanPlayCC(FC_CrowdControlInfo& CrowdControlInfo)
{
	return true;
}

void UC_CrowdControlComponent::PlayCC(FC_CrowdControlInfo& CrowdControlInfo)
{
	OnStartCC(CrowdControlInfo);
	
	CrowdControlInfos.Add(CrowdControlInfo);
	OnStartCCDelegate.Broadcast(CrowdControlInfo.CrowdControlType);
}

void UC_CrowdControlComponent::StopCC(FC_CrowdControlInfo& CrowdControlInfo)
{
	OnStopCC(CrowdControlInfo);
	
	OnEndCCDelegate.Broadcast(CrowdControlInfo.CrowdControlType);
}

void UC_CrowdControlComponent::OnStartCC(FC_CrowdControlInfo& CrowdControlInfo)
{
	FC_CrowdControlTableRow* CrowdControlTableRow = FC_GameUtil::GetCrowdControlData(CrowdControlInfo.CrowdControlDataId);
	check(CrowdControlTableRow);

	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
	check(CharacterInterface);
	
	if (CrowdControlInfo.CrowdControlType == EC_CrowdControlType::Pushback)
	{
		const FVector StartPos = CrowdControlInfo.StartPos;
		const FVector CauserPos = CrowdControlInfo.CauserPos;

		const FVector ForceDir = (StartPos - CauserPos).GetSafeNormal();
		const float Power = CrowdControlTableRow->Property_0;

		if (AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController()))
		{
			AIController->StopMovement();
		}

		OwnerCharacter->LaunchCharacter(ForceDir * Power, true, true);
	}
	
	if (TObjectPtr<UNiagaraSystem> NiagaraSystem = CrowdControlTableRow->CrowdControlFX)
		CrowdControlInfo.SpawnedFX = FC_GameUtil::SpawnEffectAttached(NiagaraSystem, OwnerCharacter->GetMesh(), FName(TEXT("root")), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);

	if (UC_CharacterDataAsset* CharacterDataAsset = CharacterInterface->GetCharacterDataAsset())
	{
		if (TObjectPtr<UAnimMontage> AnimMontage = CharacterDataAsset->KnockbackAnim)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
			check(SkeletalMeshComponent);

			UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
			check(AnimInstance);

			AnimInstance->StopAllMontages(0.2f);
			AnimInstance->Montage_Play(AnimMontage);
		}
	}
}

void UC_CrowdControlComponent::OnStopCC(FC_CrowdControlInfo& CrowdControlInfo)
{
	if (CrowdControlInfo.SpawnedFX && CrowdControlInfo.SpawnedFX->IsActive())
		CrowdControlInfo.SpawnedFX->DestroyComponent();
}

bool UC_CrowdControlComponent::IsCrowdControlled()
{
	return !CrowdControlInfos.IsEmpty();
}
