
#include "C_BattleComponent.h"

#include "C_ActionComponent.h"
#include "C_AimComponent.h"
#include "Engine/DamageEvents.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Character/C_CharacterBase.h"
#include "ProjectC/Data/C_CharacterDataAsset.h"
#include "ProjectC/Data/C_PlayerDataAsset.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"
#include "ProjectC/Utils/C_GameUtil.h"

UC_BattleComponent::UC_BattleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceInterval = 0.01f;
}


void UC_BattleComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = CastChecked<ACharacter>(GetOwner());

	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(OwnerCharacter);
	check(CharacterInterface);

	UC_CharacterDataAsset* CharacterData = CharacterInterface->GetCharacterDataAsset();
	check(CharacterData);

	TArray<FC_WeaponData>& WeaponDatas = CharacterData->WeaponIds;

	for (int32 i = 0; i < WeaponDatas.Num(); ++i)
	{
		Weapons.Add(WeaponDatas[i]);
	}

	CurWeaponIdx = -1;
	SwapWeapon();
}

void UC_BattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bTracing)
		return;

	TraceElapsedTime += DeltaTime;
	if (TraceElapsedTime < TraceInterval)
		return;

	TraceElapsedTime = 0.f;

	AC_CharacterBase* Character = Cast<AC_CharacterBase>(GetOwner());
	check(Character);

	IC_CharacterInterface* CharacterInterface = Cast<IC_CharacterInterface>(Character);
	check(CharacterInterface);

	UC_CharacterDataAsset* CharacterDataAsset = CharacterInterface->GetCharacterDataAsset();
	check(CharacterDataAsset);

	const USkeletalMeshComponent* Mesh = Character->GetMesh();
	check(Mesh);

	const UWorld* World = GetWorld();
	check(World);

	FVector CurStartBoneLocation = FVector::ZeroVector;
	FVector CurEndBoneLocation = FVector::ZeroVector;

	if (HasWeapon())
	{
		if (IC_CharacterInterface* Interface = Cast<IC_CharacterInterface>(GetOwner()))
		{
			UStaticMeshComponent* WeaponMesh = bTraceRightWeapon? Interface->GetWeapon_R_StaticMeshComponent() : Interface->GetWeapon_L_StaticMeshComponent();
			check(WeaponMesh);

			CurStartBoneLocation = WeaponMesh->GetSocketLocation(TraceStartBoneName);
			CurEndBoneLocation = WeaponMesh->GetSocketLocation(TraceEndBoneName);
		}
	}
	else
	{
		CurStartBoneLocation = Mesh->GetBoneLocation(TraceStartBoneName);
		CurEndBoneLocation = Mesh->GetBoneLocation(TraceEndBoneName);
	}

	// Trace 할 라인들 모음
	TArray<TPair<FVector, FVector>> TraceLines;
	TraceLines.Emplace(PrevStartBoneLocation, CurStartBoneLocation);  
	TraceLines.Emplace(PrevEndBoneLocation, CurEndBoneLocation);
	TraceLines.Emplace(PrevStartBoneLocation, CurEndBoneLocation);    
	TraceLines.Emplace(PrevEndBoneLocation, CurStartBoneLocation);    
	TraceLines.Emplace(CurStartBoneLocation, CurEndBoneLocation);

	int32 SegmentCount = 3;
	
	// 💡 추가: 분할 점 기반 연결선
	for (int32 i = 1; i < SegmentCount; ++i)
	{
		const float Alpha = static_cast<float>(i) / SegmentCount;

		const FVector PrevMid = FMath::Lerp(PrevStartBoneLocation, PrevEndBoneLocation, Alpha);
		const FVector CurrMid = FMath::Lerp(CurStartBoneLocation, CurEndBoneLocation, Alpha);

		TraceLines.Emplace(PrevMid, CurrMid);
	}
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	for (const auto& Line : TraceLines)
	{
		FHitResult HitResult;
		if (World->LineTraceSingleByChannel(HitResult, Line.Key, Line.Value, FC_GameUtil::GetAttackCollisionChannel(Character->CharacterDataId), Params))
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor && !DamagedActor.Contains(HitActor))
			{
				if (AC_CharacterBase* HitCharacter = Cast<AC_CharacterBase>(HitActor))
				{
					UE_LOG(LogProjectC, Log, TEXT("Hit!!"));

					DamagedActor.Add(HitActor);

					const float Damage = Character->StatComponent->GetTotalStat().Attack;

					FDamageEvent DamageEvent;
					HitActor->TakeDamage(Damage, DamageEvent, Character->GetController(), Character);

					FC_GameUtil::CameraShake();
					
					FC_GameUtil::PlayHitStop(this, 0.15f, 0.0f);
				}
			}
		}

		// DebugDraw
		//DrawDebugLine(World, Line.Key, Line.Value, FColor::Red, false, 3.f, 0, 1.f);
	}

	// Prev 갱신
	PrevStartBoneLocation = CurStartBoneLocation;
	PrevEndBoneLocation = CurEndBoneLocation;
}

void UC_BattleComponent::StartTraceWithWeapon(bool bRight)
{
	bTracing = true;
	bTraceRightWeapon = bRight;

	FC_WeaponTableRow* WeaponTableRow = bRight ? Weapon_R_TableRow : Weapon_L_TableRow;
	if (!WeaponTableRow)
		return;
	
	TraceStartBoneName = WeaponTableRow->TraceStartSocketName;
	TraceEndBoneName = WeaponTableRow->TraceEndSocketName;

	if (IC_CharacterInterface* Interface = Cast<IC_CharacterInterface>(GetOwner()))
	{
		UStaticMeshComponent* WeaponMesh = bTraceRightWeapon? Interface->GetWeapon_R_StaticMeshComponent() : Interface->GetWeapon_L_StaticMeshComponent();
		check(WeaponMesh);

		PrevStartBoneLocation = WeaponMesh->GetSocketLocation(TraceStartBoneName);
		PrevEndBoneLocation = WeaponMesh->GetSocketLocation(TraceEndBoneName);
	}
}

void UC_BattleComponent::StartTrace(FName InTraceStartBoneName, FName InTraceEndBoneName)
{
	bTracing = true;
	
	TraceStartBoneName = InTraceStartBoneName;
	TraceEndBoneName = InTraceEndBoneName;
	
	const ACharacter* Character = CastChecked<ACharacter>(GetOwner());
	const USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
	check(SkeletalMeshComponent);
	
	PrevStartBoneLocation = SkeletalMeshComponent->GetSocketLocation(TraceStartBoneName);
	PrevEndBoneLocation = SkeletalMeshComponent->GetSocketLocation(TraceEndBoneName);
}

void UC_BattleComponent::EndTrace()
{
	DamagedActor.Empty();
	bTracing = false;
	TraceElapsedTime = 0.f;
}

bool UC_BattleComponent::CanSwapWeapon()
{
	const IC_CharacterInterface* CharacterInterface = CastChecked<IC_CharacterInterface>(GetOwner());
	UC_SkillComponent* SkillComponent = CharacterInterface->GetSkillComponent();
	check(SkillComponent);

	if (SkillComponent->IsPlayingSkill())
		return false;

	if (IC_PlayerCharacterInterface* PlayerCharacterInterface = Cast<IC_PlayerCharacterInterface>(GetOwner()))
	{
		UC_ActionComponent* ActionComponent = PlayerCharacterInterface->GetActionComponent();
		check(ActionComponent);

		if (ActionComponent->IsInSpecialAction)
		{
			return false;
		}
	}
	
	return true;
}

void UC_BattleComponent::SwapWeapon()
{
	if (!CanSwapWeapon())
		return;
	
	CurWeaponIdx++;

	const int32 MaxWeaponIdx = Weapons.Num();

	if (CurWeaponIdx >= MaxWeaponIdx)
		CurWeaponIdx = 0;

	if (!Weapons.IsValidIndex(CurWeaponIdx))
		return;

	const FC_WeaponData& CurWeaponIds = Weapons[CurWeaponIdx];

	CharacterStanceType = static_cast<EC_CharacterStanceType>(CurWeaponIdx);

	UnEquipWeapon();
	
	EquipWeapon(CurWeaponIds.WeaponId_L, false);
	EquipWeapon(CurWeaponIds.WeaponId_R, true);
}

void UC_BattleComponent::EquipWeapon(uint8 InWeaponId, bool bRightHand)
{
	if (bRightHand)
		Weapon_R_TableRow = FC_GameUtil::GetWeaponData(InWeaponId);
	else
		Weapon_L_TableRow = FC_GameUtil::GetWeaponData(InWeaponId);
	
	if ((bRightHand && !Weapon_R_TableRow)|| !bRightHand && !Weapon_L_TableRow)
	{
		UnEquipWeapon();
		return;
	}
	
	if (IC_CharacterInterface* Interface = Cast<IC_CharacterInterface>(GetOwner()))
	{
		UC_CharacterDataAsset* CharacterData = Interface->GetCharacterDataAsset();
		check(CharacterData);

		FC_WeaponTableRow* WeaponTableRow = bRightHand ? Weapon_R_TableRow : Weapon_L_TableRow;
		
		FName WeaponSocketName = bRightHand? CharacterData->WeaponSocketName_R : CharacterData->WeaponSocketName_L;

		const ACharacter* Character = CastChecked<ACharacter>(GetOwner());
		USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
		check(SkeletalMeshComponent);
		
		UStaticMeshComponent* WeaponStaticMeshComponent = bRightHand? Interface->GetWeapon_R_StaticMeshComponent() : Interface->GetWeapon_L_StaticMeshComponent();
		check(WeaponStaticMeshComponent);

		WeaponStaticMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		WeaponStaticMeshComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, WeaponSocketName);
		
		WeaponStaticMeshComponent->SetRelativeLocation(WeaponTableRow->RelativePos);
		WeaponStaticMeshComponent->SetRelativeRotation(WeaponTableRow->RelativeRot);
		
		WeaponStaticMeshComponent->SetStaticMesh(WeaponTableRow->WeaponMesh);
		WeaponStaticMeshComponent->SetVisibility(true);
	}
}

void UC_BattleComponent::UnEquipWeapon()
{
	Weapon_L_TableRow = nullptr;
	Weapon_R_TableRow = nullptr;

	if (IC_CharacterInterface* Interface = Cast<IC_CharacterInterface>(GetOwner()))
	{
		UStaticMeshComponent* Weapon_L_StaticMeshComponent = Interface->GetWeapon_L_StaticMeshComponent();
		check(Weapon_L_StaticMeshComponent);
		
		UStaticMeshComponent* Weapon_R_StaticMeshComponent = Interface->GetWeapon_R_StaticMeshComponent();
		check(Weapon_R_StaticMeshComponent);

		Weapon_L_StaticMeshComponent->SetStaticMesh(nullptr);
		Weapon_L_StaticMeshComponent->SetVisibility(false);

		Weapon_R_StaticMeshComponent->SetStaticMesh(nullptr);
		Weapon_R_StaticMeshComponent->SetVisibility(false);
	}
}

bool UC_BattleComponent::HasWeapon()
{
	if (Weapon_L_TableRow || Weapon_R_TableRow)
		return true;

	return false;
}

void UC_BattleComponent::FireProjectile()
{
	
}

