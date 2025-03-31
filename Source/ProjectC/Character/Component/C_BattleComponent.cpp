
#include "C_BattleComponent.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/Character/C_CharacterBase.h"

UC_BattleComponent::UC_BattleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceInterval = 0.01f;
}


void UC_BattleComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UC_BattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTracing)
	{
		TraceElapsedTime += DeltaTime;
		if (TraceElapsedTime > TraceInterval)
		{
			TraceElapsedTime = 0.f;

			AC_CharacterBase* ClassCharacter = Cast<AC_CharacterBase>(GetOwner());
			if (!ClassCharacter)
				return;

			UC_StatComponent* StatComponent = ClassCharacter->StatComponent;
			if (!StatComponent)
				return;

			const USkeletalMeshComponent* SkeletalMeshComponent = ClassCharacter->GetMesh();
			if (!SkeletalMeshComponent)
				return;

			const UWorld* CurrentWorld = GetWorld();
			if (!CurrentWorld)
				return;
			
			const FVector CurrentLocation = SkeletalMeshComponent->GetBoneLocation(TraceBoneName);

			FRotator Rot = (PrevLocation - CurrentLocation).Rotation();
			const FRotationMatrix OriginalMatrix(Rot);
			const FVector RightVector = OriginalMatrix.GetUnitAxis(EAxis::Y);
			const FQuat OffsetQuat = FQuat(RightVector, FMath::DegreesToRadians(90));
			const FQuat NewQuat = OffsetQuat * Rot.Quaternion();
			
			Rot = NewQuat.Rotator();
			const float Height = (PrevLocation - CurrentLocation).Length();

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(GetOwner());
			
			TArray<FHitResult> HitResults;
			CurrentWorld->SweepMultiByChannel(HitResults, PrevLocation, CurrentLocation,
				Rot.Quaternion(), ECC_Pawn, FCollisionShape::MakeCapsule(20.f, Height * 0.5f), QueryParams);

			DrawDebugCapsule(CurrentWorld, FMath::Lerp(CurrentLocation, PrevLocation, 0.5f), Height * 0.5f, 20.f, Rot.Quaternion(),
				FColor::Red, false, 3.f);
			
			if (HitResults.Num() != 0 )
			{
				for (FHitResult& Result : HitResults)
				{
					if (!DamagedActor.Contains(Result.GetActor()))
					{
						if (AC_CharacterBase* Character = Cast<AC_CharacterBase>(Result.GetActor()))
						{
							UE_LOG(LogProjectC, Log, TEXT("Hit!!"));
							DamagedActor.Add(Result.GetActor());

							const float AttackDamage = StatComponent->GetTotalStat().Attack;

							FDamageEvent DamageEvent;
							Result.GetActor()->TakeDamage(AttackDamage, DamageEvent, ClassCharacter->GetController(), ClassCharacter);

							SpawnEffect(EEffectType::Hit, Result.ImpactPoint);
						}
					}
				}
			}

			PrevLocation = CurrentLocation;
		}
	}
}

void UC_BattleComponent::StartTrace(FName InTraceBoneName)
{
	TraceBoneName = InTraceBoneName;
	bTracing = true;

	if (const AC_CharacterBase* ClassCharacter = Cast<AC_CharacterBase>(GetOwner()))
	{
		if (const USkeletalMeshComponent* SkeletalMeshComponent = ClassCharacter->GetMesh())
		{
			PrevLocation = SkeletalMeshComponent->GetBoneLocation(TraceBoneName);
		}
	}
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
		StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/Assets/ParagonCrunch/FX/Particles/Abilities/Hook/FX/P_Crunch_Hook_Impact.P_Crunch_Hook_Impact")));

		if (LoadedParticle)
			UGameplayStatics::SpawnEmitterAtLocation(CurrentWorld, LoadedParticle, InHitLocation, FRotator::ZeroRotator);
	}
}

