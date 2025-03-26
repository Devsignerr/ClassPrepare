
#include "C_BattleComponent.h"

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

			const AC_CharacterBase* ClassCharacter = Cast<AC_CharacterBase>(GetOwner());
			if (!ClassCharacter)
				return;

			const USkeletalMeshComponent* SkeletalMeshComponent = ClassCharacter->GetMesh();
			if (!SkeletalMeshComponent)
				return;

			const UWorld* CurrentWorld = GetWorld();
			if (!CurrentWorld)
				return;
			
			const FVector CurrentLocation = SkeletalMeshComponent->GetBoneLocation(TraceBoneName);

			//TODO --먼저 Rot을 넣지 않은 버전을 보여주고 나서 Rot 구하는거 보여주기--
			FRotator Rot = (PrevLocation - CurrentLocation).Rotation();
			
			const FRotationMatrix OriginalMatrix(Rot);

			// 오른쪽 벡터 얻기 (로컬 기준)
			const FVector RightVector = OriginalMatrix.GetUnitAxis(EAxis::Y);
			
			// 오른쪽으로 90도 회전한 회전 계산
			const FQuat OffsetQuat = FQuat(RightVector, FMath::DegreesToRadians(90));
			
			// 새로운 회전 계산
			// Quaternion A * B 의 의미:
			// "B를 먼저 하고 → 그 다음 A를 적용한다"
			// 즉, 오른쪽부터 먼저 적용되는 거야!
			const FQuat NewQuat = OffsetQuat * Rot.Quaternion();
			
			// 결과
			Rot = NewQuat.Rotator();

			//TODO --먼저 상수 Height 값 넣는거 보여주고 나서 Height 구하는거 보여주기 --
			const float Height = (PrevLocation - CurrentLocation).Length();

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(GetOwner());
			
			TArray<FHitResult> HitResults;
			CurrentWorld->SweepMultiByChannel(HitResults, PrevLocation, CurrentLocation,
				Rot.Quaternion(), ECC_Pawn, FCollisionShape::MakeCapsule(20.f, Height * 0.5f), QueryParams);

			DrawDebugCapsule(CurrentWorld, FMath::Lerp(CurrentLocation, PrevLocation, 0.5f), Height * 0.5f, 20.f, Rot.Quaternion(),
				FColor::Red, false, 3.f);

			//TODO --먼저 로그띄우는것만 보여주고, 나중에 DamageActor 멤버 만들어서 한번 충돌한 액터는 다시 충돌안하게 하기 --
			if (HitResults.Num() != 0 )
			{
				for (FHitResult& Result : HitResults)
				{
					if (!DamagedActor.Contains(Result.GetActor()))
					{
						UE_LOG(LogProjectC, Log, TEXT("Hit!!"));
						DamagedActor.Add(Result.GetActor());

						SpawnEffect(EEffectType::Hit, Result.ImpactPoint);
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

