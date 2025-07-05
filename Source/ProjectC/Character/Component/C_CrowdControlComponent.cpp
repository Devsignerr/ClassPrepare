#include "C_CrowdControlComponent.h"

#include "AIController.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
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
	ProcessCC();
	
	CrowdControlInfo.ElapsedTime += DeltaTime;
	if (CrowdControlInfo.bValid && CrowdControlInfo.ElapsedTime > CrowdControlInfo.LifeTime)
	{
		StopCC();
	}
}

void UC_CrowdControlComponent::ProcessCC()
{
	
}

void UC_CrowdControlComponent::RequestPlayCC(uint32 CrowdControlId, AActor* Causer)
{
	FC_CrowdControlTableRow* CrowdControlTableRow = FC_GameUtil::GetCrowdControlData(CrowdControlId);
	check(CrowdControlTableRow);
	
	FC_CrowdControlInfo Info;
	Info.LifeTime = CrowdControlTableRow->Duration;
	Info.ElapsedTime = 0.f;
	Info.CrowdControlType = CrowdControlTableRow->CrowdControlType;
	Info.CrowdControlDataId = CrowdControlTableRow->DataId;
	Info.Causer = Causer;

	Info.CauserPos = Causer->GetActorLocation();
	Info.CauserRot = Causer->GetActorRotation();
	Info.StartPos = OwnerCharacter->GetActorLocation();
	Info.StartRot = OwnerCharacter->GetActorRotation();
	
	if (!CanPlayCC(Info))
		return;

	//이미 실행중인 CC가 있을경우 ID가 다를때에만 Stop처리
	if (CrowdControlInfo.bValid)
	{
		if (CrowdControlInfo.CrowdControlDataId != CrowdControlId)
			StopCC();
	}

	PlayCC(Info);
}

bool UC_CrowdControlComponent::CanPlayCC(FC_CrowdControlInfo& Info)
{
	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
	check(CharacterInterface);

	if (CharacterInterface->IsDead())
		return false;
	
	return true;
}

void UC_CrowdControlComponent::PlayCC(FC_CrowdControlInfo& Info)
{
	const bool ShouldPlayFx = Info.CrowdControlDataId != CrowdControlInfo.CrowdControlDataId || !CrowdControlInfo.bValid;

	if (ShouldPlayFx)
		PlayFX(Info);
	else
		Info.SpawnedFX = CrowdControlInfo.SpawnedFX;

	CrowdControlInfo = Info;
	OnStartCC();
	
	CrowdControlInfo.bValid = true;
	OnStartCCDelegate.Broadcast(CrowdControlInfo.CrowdControlType, CrowdControlInfo.Causer.Get());
}

void UC_CrowdControlComponent::StopCC()
{
	OnStopCC();
	
	CrowdControlInfo.bValid = false;
	OnEndCCDelegate.Broadcast(CrowdControlInfo.CrowdControlType, CrowdControlInfo.Causer.Get());
}

void UC_CrowdControlComponent::OnStartCC()
{
	FC_CrowdControlTableRow* CrowdControlTableRow = FC_GameUtil::GetCrowdControlData(CrowdControlInfo.CrowdControlDataId);
	check(CrowdControlTableRow);

	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
	check(CharacterInterface);

	if (AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController()))
	{
		AIController->StopMovement();
	}
	
	if (CrowdControlInfo.CrowdControlType == EC_CrowdControlType::Pushback)
	{
		const FVector StartPos = CrowdControlInfo.StartPos;
		const FVector CauserPos = CrowdControlInfo.CauserPos;

		const FVector ForceDir = (StartPos - CauserPos).GetSafeNormal();
		const float Power = CrowdControlTableRow->Property_0;
		
		//DrawDebugLine(GetWorld(), StartPos, StartPos + (-ForceDir.GetSafeNormal2D()) * 200.f, FColor::Red, false, 3.f, 20);

		OwnerCharacter->SetActorRotation((-ForceDir.GetSafeNormal2D()).Rotation());
		OwnerCharacter->LaunchCharacter(ForceDir * Power, true, true);
	}
	else if (CrowdControlInfo.CrowdControlType == EC_CrowdControlType::Stun)
	{
	
	}
	else if (CrowdControlInfo.CrowdControlType == EC_CrowdControlType::Freeze)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
		check(SkeletalMeshComponent);

		OwnerCharacter->GetCharacterMovement()->DisableMovement();
		SkeletalMeshComponent->SetComponentTickEnabled(false);
	}
	
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

void UC_CrowdControlComponent::OnStopCC()
{
	FC_CrowdControlTableRow* CrowdControlTableRow = FC_GameUtil::GetCrowdControlData(CrowdControlInfo.CrowdControlDataId);
	check(CrowdControlTableRow);
	
	if (CrowdControlInfo.SpawnedFX && CrowdControlInfo.SpawnedFX->IsActive())
		CrowdControlInfo.SpawnedFX->Deactivate();

	if (CrowdControlTableRow->MaterialInterface)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
		check(SkeletalMeshComponent);

		SkeletalMeshComponent->SetOverlayMaterial(nullptr);
	}
	
	if (CrowdControlInfo.CrowdControlType == EC_CrowdControlType::Freeze)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
		check(SkeletalMeshComponent);
		
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		SkeletalMeshComponent->SetComponentTickEnabled(true);
	}
}

void UC_CrowdControlComponent::PlayFX(FC_CrowdControlInfo& Info)
{
	FC_CrowdControlTableRow* CrowdControlTableRow = FC_GameUtil::GetCrowdControlData(Info.CrowdControlDataId);
	check(CrowdControlTableRow);
	
	if (UNiagaraSystem* NiagaraSystem = CrowdControlTableRow->CrowdControlFX)
	{
		FVector RelativePos = FVector(0.f, 0.f, OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		Info.SpawnedFX = FC_GameUtil::SpawnEffectAttached(NiagaraSystem, OwnerCharacter->GetCapsuleComponent(), NAME_None, RelativePos, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}
	
	if (CrowdControlTableRow->MaterialInterface)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
		check(SkeletalMeshComponent);

		SkeletalMeshComponent->SetOverlayMaterial(CrowdControlTableRow->MaterialInterface);
	}
}

void UC_CrowdControlComponent::StopFX()
{
	
}

bool UC_CrowdControlComponent::IsCrowdControlled()
{
	return CrowdControlInfo.bValid;
}
