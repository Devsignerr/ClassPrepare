
#include "C_BattleComponent.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Character/C_CharacterBase.h"
#include "ProjectC/Character/C_PlayableCharacter.h"
#include "ProjectC/Data/C_PlayerDataAsset.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"
#include "ProjectC/SkillObject/C_SkillObject.h"
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

	Weapons.Add(0);
	Weapons.Add(1);
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

	AC_CharacterBase* ClassCharacter = Cast<AC_CharacterBase>(GetOwner());
	if (!ClassCharacter)
		return;

	const USkeletalMeshComponent* Mesh = ClassCharacter->GetMesh();
	if (!Mesh)
		return;

	const UWorld* World = GetWorld();
	if (!World)
		return;

	FVector CurStartBoneLocation = FVector::ZeroVector;
	FVector CurEndBoneLocation = FVector::ZeroVector;

	if (HasWeapon())
	{
		if (const IC_PlayerCharacterInterface* Interface = Cast<IC_PlayerCharacterInterface>(GetOwner()))
		{
			UStaticMeshComponent* WeaponMesh = Interface->GetWeaponStaticMeshComponent();
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
		if (World->LineTraceSingleByChannel(HitResult, Line.Key, Line.Value, ECC_Pawn, Params))
		{
			AActor* HitActor = HitResult.GetActor();

			if (HitActor && !DamagedActor.Contains(HitActor))
			{
				if (AC_CharacterBase* HitCharacter = Cast<AC_CharacterBase>(HitActor))
				{
					UE_LOG(LogProjectC, Log, TEXT("Hit!!"));

					DamagedActor.Add(HitActor);

					const float Damage = ClassCharacter->StatComponent->GetTotalStat().Attack;

					FDamageEvent DamageEvent;
					HitActor->TakeDamage(Damage, DamageEvent, ClassCharacter->GetController(), ClassCharacter);

					SpawnEffect(EEffectType::Hit, HitResult.ImpactPoint);
				}
			}
		}

		// DebugDraw
		DrawDebugLine(World, Line.Key, Line.Value, FColor::Red, false, 3.f, 0, 1.f);
	}

	// Prev 갱신
	PrevStartBoneLocation = CurStartBoneLocation;
	PrevEndBoneLocation = CurEndBoneLocation;
}

void UC_BattleComponent::StartTraceWithWeapon()
{
	bTracing = true;
	
	TraceStartBoneName = WeaponTableRow->TraceStartSocketName;
	TraceEndBoneName = WeaponTableRow->TraceEndSocketName;

	if (const IC_PlayerCharacterInterface* Interface = Cast<IC_PlayerCharacterInterface>(GetOwner()))
	{
		UStaticMeshComponent* WeaponStaticMeshComponent = Interface->GetWeaponStaticMeshComponent();
		check(WeaponStaticMeshComponent);

		PrevStartBoneLocation = WeaponStaticMeshComponent->GetSocketLocation(TraceStartBoneName);
		PrevEndBoneLocation = WeaponStaticMeshComponent->GetSocketLocation(TraceEndBoneName);
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

void UC_BattleComponent::SpawnEffect(EEffectType InEffectType, FVector InHitLocation)
{
	const UWorld* CurrentWorld = GetWorld();
	if (!CurrentWorld)
		return;
	
	if (InEffectType == EEffectType::Hit)
	{
		//이번만 예외로 적용 후 데이터 분리
		UParticleSystem* LoadedParticle = Cast<UParticleSystem>(
		StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/ParagonKallari/FX/Particles/Kallari/Abilities/Primary/FX/P_Kallari_Melee_SucessfulImpact.P_Kallari_Melee_SucessfulImpact")));

		if (LoadedParticle)
			UGameplayStatics::SpawnEmitterAtLocation(CurrentWorld, LoadedParticle, InHitLocation, FRotator::ZeroRotator);

		FC_GameUtil::CameraShake();
	}
}

void UC_BattleComponent::SwapWeapon()
{
	CurWeaponIdx++;

	const int32 MaxWeaponIdx = Weapons.Num();

	if (CurWeaponIdx >= MaxWeaponIdx)
		CurWeaponIdx = 0;

	const uint8 CurWeaponId = Weapons[CurWeaponIdx];

	CharacterStanceType = static_cast<EC_CharacterStanceType>(CurWeaponIdx);
	
	UnEquipWeapon();
	EquipWeapon(CurWeaponId);
}

void UC_BattleComponent::EquipWeapon(uint8 InWeaponId)
{
	WeaponTableRow = FC_GameUtil::GetWeaponData(InWeaponId);
	if (!WeaponTableRow)
	{
		UnEquipWeapon();
		return;
	}
	
	FName WeaponSocketName = NAME_None;
	
	if (const IC_PlayerCharacterInterface* Interface = Cast<IC_PlayerCharacterInterface>(GetOwner()))
	{
		UC_PlayerDataAsset* PlayerData = Interface->GetPlayerData();
		check(PlayerData);

		WeaponSocketName = PlayerData->WeaponSocketName;

		const ACharacter* Character = CastChecked<ACharacter>(GetOwner());
		USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
		check(SkeletalMeshComponent);
		
		UStaticMeshComponent* WeaponStaticMeshComponent = Interface->GetWeaponStaticMeshComponent();
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
	WeaponTableRow = nullptr;

	if (const IC_PlayerCharacterInterface* Interface = Cast<IC_PlayerCharacterInterface>(GetOwner()))
	{
		UStaticMeshComponent* WeaponStaticMeshComponent = Interface->GetWeaponStaticMeshComponent();
		check(WeaponStaticMeshComponent);

		WeaponStaticMeshComponent->SetStaticMesh(nullptr);
		WeaponStaticMeshComponent->SetVisibility(false);
	}
}

bool UC_BattleComponent::HasWeapon()
{
	if (WeaponTableRow)
		return true;

	return false;
}

void UC_BattleComponent::FireProjectile()
{
	if (CharacterStanceType == EC_CharacterStanceType::Staff)
	{
		const APlayerController* PlayerController = CastChecked<APlayerController>(OwnerCharacter->GetController());

		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
		check(SkeletalMeshComponent);
	
		FVector Location = SkeletalMeshComponent->GetSocketLocation(TEXT("hand_l"));
		FRotator Rotation = PlayerController->GetControlRotation();

		FTransform Transform;
		Transform.SetLocation(Location);
		Transform.SetRotation(Rotation.Quaternion());

		AC_PlayableCharacter* PlayableCharacter = Cast<AC_PlayableCharacter>(OwnerCharacter);
		check(PlayableCharacter);
	
		AC_SkillObject* SkillObject = GetWorld()->SpawnActorDeferred<AC_SkillObject>(PlayableCharacter->ProjectileClass, Transform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		SkillObject->OwnerCharacter = OwnerCharacter.Get();
		SkillObject->FinishSpawning(Transform);
	}
}

