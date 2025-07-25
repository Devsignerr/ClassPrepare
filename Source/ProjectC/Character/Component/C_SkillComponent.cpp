
#include "C_SkillComponent.h"
#include "C_CrowdControlComponent.h"
#include "NiagaraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "ProjectC/Data/C_TableRows.h"
#include "..\..\Interface\C_CharacterInterface.h"
#include "Engine/StaticMeshActor.h"
#include "ProjectC/Character/C_CharacterBase.h"
#include "ProjectC/Utils/C_GameUtil.h"
#include "ProjectC/SkillObject/C_SkillObject.h"

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
		ProcessSkill(DeltaTime, SkillInfo);
		
		if (SkillInfo.ElapsedTime > SkillInfo.LifeTime)
		{
			SkillToCoolDown.Add(SkillInfo);
		}
	}

	for (FC_SkillInfo& SkillInfo : SkillToCoolDown)
	{
		CoolDownSkillInfos.Add(SkillInfo);
		
		OnEndSkillDelegate.Broadcast(SkillInfo.SkillDataId);
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

void UC_SkillComponent::RequestStopSkill(uint32 SkillId)
{
	
}

void UC_SkillComponent::FindTargets(uint32 SkillId, TArray<TWeakObjectPtr<AActor>>& Targets)
{
	TArray<AActor*> Enemys;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Enemys);

	const float SkillRange = FC_GameUtil::GetSkillData(SkillId)->SkillRange;
	const FVector OwnerCharacterPos = OwnerCharacter->GetActorLocation();

	for (AActor* Enemy : Enemys)
	{
		if (Enemy != GetOwner())
		{
			if (IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(Enemy))
			{
				if (CharacterInterface->IsDead())
					continue;
			}

			bool bWallDetected = false;
			
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(OwnerCharacter.Get());
			
			TArray<FHitResult> HitResults;
			if (GetWorld()->LineTraceMultiByChannel(HitResults, OwnerCharacter->GetActorLocation(), Enemy->GetActorLocation(), ECC_WorldStatic, Params))
			{
				for (FHitResult& Result : HitResults)
				{
					if (Result.GetActor()->IsA(AStaticMeshActor::StaticClass()))
					{
						bWallDetected = true;
						break;
					}
				}
			}

			if (bWallDetected)
			{
				continue;
			}
			
			if (FVector::Dist(OwnerCharacterPos, Enemy->GetActorLocation()) <= SkillRange)
			{
				Targets.Add(Enemy);
			}
		}
	}
}

bool UC_SkillComponent::CanPlaySkill(uint32 SkillId)
{
	const FC_SkillTableRow* SkillTableRow = FC_GameUtil::GetSkillData(SkillId);
	if (!SkillTableRow)
		return false;
	
	return true;
}

void UC_SkillComponent::PlaySkill(FC_SkillInfo& SkillInfo)
{
	CurrentPlayingSkillInfos.Add(SkillInfo);

	OnStartSkillDelegate.Broadcast(SkillInfo.SkillDataId);
}

void UC_SkillComponent::CalcSkillTime(uint32 SKillId, float& SkillLifeTime, TArray<FC_ExecInfo>& ExecInfos)
{
	for (int32 i = 0; i < ExecInfos.Num(); ++i)
	{
		FC_ExecData& ExecData = FC_GameUtil::GetSkillData(SKillId)->ExecDatas[i];
		FC_ExecInfo& ExecInfo = ExecInfos[i];
		ExecInfo.ExecData = &ExecData;
		ExecInfo.ExecSequence = i;
		ExecInfo.AnimStartTime = SkillLifeTime;
		ExecInfo.ExecStartTime = SkillLifeTime + ExecData.DelayTime;
		ExecInfo.EndTime = ExecInfo.ExecStartTime + FC_GameUtil::GetExecData(ExecData.ExecDataId)->Duration;

		SkillLifeTime += ExecData.DelayTime + FC_GameUtil::GetExecData(ExecData.ExecDataId)->Duration;
	}
}

void UC_SkillComponent::ProcessSkill(float DeltaTime, FC_SkillInfo& SkillInfo)
{
	float ElapsedTime = SkillInfo.ElapsedTime; 
	
	for (FC_ExecInfo& ExecInfo : SkillInfo.ExecInfos)
	{
		if (ExecInfo.bExecFinished)
			continue;

		FC_SkillTableRow* SkillTableRow = FC_GameUtil::GetSkillData(SkillInfo.SkillDataId);
		check(SkillTableRow);
		
		FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
		check(ExecTableRow);
		
		if (ElapsedTime >= ExecInfo.AnimStartTime && !ExecInfo.bAnimStarted)
		{
			UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
			check(AnimInstance);

			ExecInfo.bAnimStarted = true;

			AnimInstance->StopAllMontages(0.1f);
			AnimInstance->Montage_Play(ExecTableRow->SkillAnim);
		}

		if (ElapsedTime >= ExecInfo.ExecStartTime && ElapsedTime <= ExecInfo.EndTime)
		{
			ExecInfo.ElapsedTime += DeltaTime;
			
			if (!ExecInfo.bExecStarted)
			{
				ExecInfo.bExecStarted = true;
				OnStartExec(SkillInfo, ExecInfo);
			}

			if (SkillTableRow->SkillTargetingType == EC_SkillTargetingType::ChainAttack)
				ProcessChainAttackExec(DeltaTime, SkillInfo, ExecInfo, SkillInfo.SkillStartPos, SkillInfo.SkillStartRot);
			else if (SkillTableRow->SkillTargetingType == EC_SkillTargetingType::NoneTarget)
				ProcessNoneTargetExec(DeltaTime, ExecInfo, SkillInfo.SkillStartPos, SkillInfo.SkillStartRot);
			else if (SkillTableRow->SkillTargetingType == EC_SkillTargetingType::Multiple)
				ProcessMultipleExec(DeltaTime, SkillInfo, ExecInfo, SkillInfo.SkillStartPos, SkillInfo.SkillStartRot);
		}

		if (ExecInfo.EndTime <= ElapsedTime)
		{
			ExecInfo.bExecFinished = true;
			OnEndExec(SkillInfo, ExecInfo);
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

		float CurveAlpha = ExecInfo.ElapsedTime / Duration;
		float PosAlpha = 0;
		
		if (ExecTableRow->ExecCurve)
			PosAlpha = ExecTableRow->ExecCurve->GetFloatValue(CurveAlpha);
		else
			PosAlpha = CurveAlpha;

		FVector CurrentPos = OwnerCharacter->GetActorLocation();
		FVector NewPos = ExecInfo.ExecStartPos + ExecInfo.ExecStartRot.Vector() * DashRange * PosAlpha;
		NewPos.Z = CurrentPos.Z;
		
		NewPos = FC_GameUtil::FindSurfacePos(OwnerCharacter.Get(), NewPos);
		OwnerCharacter->SetActorLocation(NewPos);

		if (ExecTableRow->bSpawnCollision && CurveAlpha > 0.5f && !ExecInfo.bExecCollisionSpawned)
		{
			ExecInfo.bExecCollisionSpawned = true;

			FVector ExecCollisionPos = ExecInfo.ExecStartPos + ExecInfo.ExecStartRot.Vector() * DashRange / 2.f;
			FRotator ExecCollisionRot = ExecInfo.ExecStartRot;
			
			EC_ExecCollisionType CollisionType = ExecTableRow->ExecCollisionType;
			if (CollisionType == EC_ExecCollisionType::Box)
			{
				float BoxHeight = ExecTableRow->ExecCollisionProperty_0;
				float BoxWidth = ExecTableRow->ExecCollisionProperty_1;
				float BoxLength = DashRange / 2.f;
				
				FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(BoxLength, BoxWidth, BoxHeight));
				CheckCollision(ExecInfo, CollisionShape, ExecCollisionPos, ExecCollisionRot);
			}
		}
	}
	else if (ExecTableRow->ExecType == EC_ExecType::Dot)
	{
		FVector CurrentPos = FVector::ZeroVector;
		if (ExecTableRow->SkillPosBoneName != NAME_None)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
			check(SkeletalMeshComponent);
	
			CurrentPos = SkeletalMeshComponent->GetSocketLocation(ExecTableRow->SkillPosBoneName);
		}
		else
		{
			CurrentPos = OwnerCharacter->GetActorLocation();
		}
		FRotator DotDir = OwnerCharacter->GetControlRotation();
		ExecInfo.IntervalElapsedTime += DeltaTime;

		float Interval = ExecTableRow->ExecProperty_0;

		if (ExecTableRow->bSpawnCollision && ExecInfo.IntervalElapsedTime >= Interval && !ExecInfo.bExecCollisionSpawned)
		{
			ExecInfo.IntervalElapsedTime = 0.f;

			FVector ExecCollisionPos = CurrentPos + DotDir.Vector() * ExecTableRow->ExecCollisionProperty_2;
			FRotator ExecCollisionRot = DotDir;
			
			EC_ExecCollisionType CollisionType = ExecTableRow->ExecCollisionType;
			if (CollisionType == EC_ExecCollisionType::Box)
			{
				float BoxHeight = ExecTableRow->ExecCollisionProperty_0;
				float BoxWidth = ExecTableRow->ExecCollisionProperty_1;
				float BoxLength = ExecTableRow->ExecCollisionProperty_2;
				
				FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(BoxLength, BoxWidth, BoxHeight));
				CheckCollision(ExecInfo, CollisionShape, ExecCollisionPos, ExecCollisionRot);
			}
		}
	}
	else if (ExecTableRow->ExecType == EC_ExecType::Pushback)
	{
		if (ExecTableRow->bSpawnCollision && !ExecInfo.bExecCollisionSpawned)
		{
			ExecInfo.bExecCollisionSpawned = true;
			
			FVector ExecCollisionPos = ExecInfo.ExecStartPos;
			FRotator ExecCollisionRot = ExecInfo.ExecStartRot;
			float SphereRadius = ExecTableRow->ExecCollisionProperty_0;
			FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SphereRadius);
		
			UWorld* World = GetWorld();
			check(World);

			//DrawDebugSphere(World, ExecCollisionPos, SphereRadius, 10, FColor::Red, false, 3.f);
		
			TArray<FOverlapResult> OverlapResults;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(OwnerCharacter.Get());

			EC_ExecCollisionType CollisionType = ExecTableRow->ExecCollisionType;
			if (CollisionType == EC_ExecCollisionType::Sphere)
			{
				CheckCollision(ExecInfo, CollisionShape, ExecCollisionPos, ExecCollisionRot);
			}
		}
	}
}

void UC_SkillComponent::ProcessChainAttackExec(float DeltaTime, FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo, FVector StartPos, FRotator StartRot)
{
	FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
	check(ExecTableRow);

	TArray<TWeakObjectPtr<AActor>>& Targets = SkillInfo.Targets;

	uint32 TargetIndex = ExecInfo.ExecSequence % Targets.Num();
	TWeakObjectPtr<ACharacter> Target = Cast<ACharacter>(Targets[TargetIndex]);
	if (!Target.IsValid())
		return;
	
	if (ExecTableRow->ExecType == EC_ExecType::DashToTarget)
	{
		FVector TargetPos = ExecInfo.ExecEndPos;
		FVector CurrentPos = OwnerCharacter->GetActorLocation();

		FVector ToTargetVector = TargetPos - ExecInfo.ExecStartPos;
		FVector ToTargetDir = ToTargetVector.GetSafeNormal();

		float OverRunDistance = ExecTableRow->ExecProperty_0;

		TargetPos += ToTargetDir * OverRunDistance;

		float ToTargetLength = (TargetPos - ExecInfo.ExecStartPos).Length();
		float Duration = ExecTableRow->Duration;

		float CurveAlpha = ExecInfo.ElapsedTime / Duration;
		float PosAlpha = ExecTableRow->ExecCurve->GetFloatValue(CurveAlpha);

		FVector NewPos = ExecInfo.ExecStartPos + ToTargetDir * ToTargetLength * PosAlpha;
		NewPos.Z = CurrentPos.Z;
		
		NewPos = FC_GameUtil::FindSurfacePos(OwnerCharacter.Get(), NewPos);
		OwnerCharacter->SetActorLocation(NewPos);
		OwnerCharacter->SetActorRotation(ExecInfo.ExecEndRot);
		
		if (ExecTableRow->bSpawnCollision && CurveAlpha > 0.5f && !ExecInfo.bExecCollisionSpawned)
		{
			ExecInfo.bExecCollisionSpawned = true;

			FVector ExecCollisionPos = ExecInfo.ExecStartPos + ToTargetDir * ToTargetLength / 2.f;
			FRotator ExecCollisionRot = ToTargetDir.Rotation();
			
			EC_ExecCollisionType CollisionType = ExecTableRow->ExecCollisionType;
			if (CollisionType == EC_ExecCollisionType::Box)
			{
				float BoxHeight = ExecTableRow->ExecCollisionProperty_0;
				float BoxWidth = ExecTableRow->ExecCollisionProperty_1;
				float BoxLength = (TargetPos - ExecInfo.ExecStartPos).Length() / 2.f;
				
				FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(BoxLength, BoxWidth, BoxHeight));
				CheckCollision(ExecInfo, CollisionShape, ExecCollisionPos, ExecCollisionRot);
			}
		}
	}
}

void UC_SkillComponent::ProcessMultipleExec(float DeltaTime, FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo, FVector StartPos, FRotator StartRot)
{
	FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
	check(ExecTableRow);
	
	if (ExecTableRow->ExecType == EC_ExecType::Projectile)
	{
		if (!ExecInfo.bExecCollisionSpawned)
		{
			ExecInfo.bExecCollisionSpawned = true;
			
			FC_SkillObjectTableRow* SkillObjectTableRow = FC_GameUtil::GetSkillObjectData(ExecTableRow->ExecProperty_0);
			check(SkillObjectTableRow);

			UClass* SkillObjectClass = SkillObjectTableRow->SkillObjectActor;
			check(SkillObjectClass);

			TArray<TWeakObjectPtr<AActor>>& Targets = SkillInfo.Targets;

			for (TWeakObjectPtr<AActor> Actor : Targets)
			{
				if (ExecTableRow->ExecType == EC_ExecType::Projectile)
				{
					FVector TargetLocation = Actor->GetActorLocation();
			
					USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
					check(SkeletalMeshComponent);

					FVector Location = FVector::ZeroVector;
					if (ExecTableRow->SkillPosBoneName != NAME_None)
					{
						Location = SkeletalMeshComponent->GetSocketLocation(ExecTableRow->SkillPosBoneName);
					}
					else
					{
						Location = OwnerCharacter->GetActorLocation();
					}
	
					FRotator Rotation = (TargetLocation - Location).Rotation();

					FTransform Transform;
					Transform.SetLocation(Location);
					Transform.SetRotation(Rotation.Quaternion());

					AC_SkillObject* SkillObject = GetWorld()->SpawnActorDeferred<AC_SkillObject>(SkillObjectClass, Transform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
					SkillObject->OwnerCharacter = OwnerCharacter.Get();
					SkillObject->SkillObjectId = ExecTableRow->ExecProperty_0;
					SkillObject->FinishSpawning(Transform);
				}
			}
		}
	}
}

void UC_SkillComponent::CheckCollision(FC_ExecInfo& ExecInfo, FCollisionShape CollisionShape, FVector Pos, FRotator Rot)
{
	FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
	check(ExecTableRow);

	UWorld* World = GetWorld();
	check(World);

	AC_CharacterBase* CharacterBase = Cast<AC_CharacterBase>(OwnerCharacter);
	check(CharacterBase);

	//DrawDebugBox(World, Pos, CollisionShape.GetExtent(), Rot.Quaternion(), FColor::Red, false, 3.f);
		
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter.Get());
	if (World->OverlapMultiByChannel(OverlapResults, Pos, Rot.Quaternion(), FC_GameUtil::GetAttackCollisionChannel(CharacterBase->CharacterDataId), CollisionShape, QueryParams))
	{
		for (FOverlapResult& Result : OverlapResults)
		{
			if (ACharacter* HitCharacter = Cast<ACharacter>(Result.GetActor()))
			{
				FDamageEvent DamageEvent;
				HitCharacter->TakeDamage(ExecTableRow->Damage, DamageEvent, OwnerCharacter->GetController(), OwnerCharacter.Get());

				FVector FXLocation = HitCharacter->GetActorLocation();
				FC_GameUtil::SpawnEffectAtLocation(GetWorld(), ExecTableRow->HitFX_Niagara, FXLocation, FRotator::ZeroRotator);
				FC_GameUtil::SpawnEffectAtLocation(GetWorld(), ExecTableRow->HitFX_Cascade, FXLocation, FRotator::ZeroRotator);

				if (ExecTableRow->bPlayCameraShake)
					FC_GameUtil::CameraShake();

				if (IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(Result.GetActor()))
				{
					UC_CrowdControlComponent* CrowdControlComponent = CharacterInterface->GetCrowdControlComponent();
					check(CrowdControlComponent);

					const float CrowdControlId = ExecTableRow->CrowdControlId;
					if (CrowdControlId > INDEX_NONE)
					{
						CrowdControlComponent->RequestPlayCC(CrowdControlId, GetOwner());
					}
				}
			}
		}
	}
}

void UC_SkillComponent::OnStartExec(FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo)
{
	FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
	check(ExecTableRow);

	USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh();
	check(SkeletalMesh);
	
	ExecInfo.ExecStartPos = OwnerCharacter->GetActorLocation();
	ExecInfo.ExecStartRot = OwnerCharacter->GetActorRotation();
	
	if (ExecTableRow->bAttachFx)
	{
		ExecInfo.AttachedFX = FC_GameUtil::SpawnEffectAttached(ExecTableRow->ExecFX_Niagara_Start, SkeletalMesh, ExecTableRow->SkillPosBoneName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}
	else
	{
		FC_GameUtil::SpawnEffectAtLocation(GetWorld(), ExecTableRow->ExecFX_Niagara_Start, ExecInfo.ExecStartPos, ExecInfo.ExecStartRot);
		FC_GameUtil::SpawnEffectAtLocation(GetWorld(), ExecTableRow->ExecFX_Cascade_Start, ExecInfo.ExecStartPos, ExecInfo.ExecStartRot);
	}

	if (ExecTableRow->MaterialInterface)
	{
		TArray<UMaterialInterface*> MaterialInterfaces = SkeletalMesh->GetMaterials();
			
		for (int32 i = 0; i < MaterialInterfaces.Num(); ++i)
		{
			UMaterialInterface* MaterialInstance = MaterialInterfaces[i];
			
			ExecInfo.OriginalMaterials.Add(MaterialInstance);

			SkeletalMesh->SetMaterial(i, ExecTableRow->MaterialInterface);
		}
	}
	
	if (ExecTableRow->ExecType == EC_ExecType::DashToTarget)
	{
		TArray<TWeakObjectPtr<AActor>>& Targets = SkillInfo.Targets;
		if (Targets.IsEmpty())
			return;

		uint32 TargetIndex = ExecInfo.ExecSequence % Targets.Num();
		TWeakObjectPtr<ACharacter> Target = Cast<ACharacter>(Targets[TargetIndex]);
		if (!Target.IsValid())
			return;
		
		FVector TargetPos = Target->GetActorLocation();

		FVector ToTargetVector = TargetPos - ExecInfo.ExecStartPos;
		FVector ToTargetDir = ToTargetVector.GetSafeNormal();

		float OverRunDistance = ExecTableRow->ExecProperty_0;
		TargetPos += ToTargetDir * OverRunDistance;

		ExecInfo.ExecEndPos = TargetPos;
		ExecInfo.ExecEndRot = ToTargetDir.GetSafeNormal2D().Rotation();
	}
}

void UC_SkillComponent::OnEndExec(FC_SkillInfo& SkillInfo, FC_ExecInfo& ExecInfo)
{
	FC_ExecTableRow* ExecTableRow = FC_GameUtil::GetExecData(ExecInfo.ExecData->ExecDataId);
	check(ExecTableRow);

	USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh();
	check(SkeletalMesh);
	
	const FVector Location = OwnerCharacter->GetActorLocation();
	const FRotator Rotation = OwnerCharacter->GetActorRotation();
	
	FC_GameUtil::SpawnEffectAtLocation(GetWorld(), ExecTableRow->ExecFX_Niagara_End, Location, Rotation);
	FC_GameUtil::SpawnEffectAtLocation(GetWorld(), ExecTableRow->ExecFX_Cascade_End, Location, Rotation);

	if (ExecInfo.AttachedFX)
	{
		ExecInfo.AttachedFX->Deactivate();
	}

	if (ExecTableRow->MaterialInterface)
	{
		TArray<UMaterialInterface*> MaterialInterfaces = SkeletalMesh->GetMaterials();
		
		for (int32 i = 0; i < MaterialInterfaces.Num(); ++i)
		{
			SkeletalMesh->SetMaterial(i, ExecInfo.OriginalMaterials[i]);
		}
	}
}
