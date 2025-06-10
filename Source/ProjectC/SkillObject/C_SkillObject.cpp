#include "C_SkillObject.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "ProjectC/Utils/C_GameUtil.h"

AC_SkillObject::AC_SkillObject()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision_Environment = CreateDefaultSubobject<USphereComponent>(TEXT("Collision_Environment"));
	RootComponent = Collision_Environment;
	
	TriggerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	TriggerCollision->SetupAttachment(RootComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent); 
}

void AC_SkillObject::BeginPlay()
{
	Super::BeginPlay();

	if (!TriggerCollision->OnComponentBeginOverlap.IsAlreadyBound(this, &ThisClass::OnBeginOverlap))
		TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);

	check(Collision_Environment);
	Collision_Environment->SetCollisionProfileName(TEXT("SkillObject_Environment"));
	Collision_Environment->SetSphereRadius(1.f);

	if (!Collision_Environment->OnComponentHit.IsAlreadyBound(this, &ThisClass::OnComponentHit))
		Collision_Environment->OnComponentHit.AddDynamic(this, &ThisClass::OnComponentHit);

	if (BounceCount > 0)
		ProjectileMovementComponent->bShouldBounce = true;
	
	PlaySound(true);
}

void AC_SkillObject::PlaySound(bool bSpawn)
{
	FVector ActorLocation = GetActorLocation();

	if (bSpawn)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SpawnSound, ActorLocation);
	else
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DeSpawnSound, ActorLocation);
	}
}

void AC_SkillObject::PlayFX(bool bSpawn, FVector InHitLocation)
{
	UParticleSystem* LoadedParticle = Cast<UParticleSystem>(
		StaticLoadObject(UParticleSystem::StaticClass(), nullptr, TEXT("/Game/ParagonKallari/FX/Particles/Kallari/Abilities/Primary/FX/P_Kallari_Melee_SucessfulImpact.P_Kallari_Melee_SucessfulImpact")));

	if (LoadedParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LoadedParticle, InHitLocation, FRotator::ZeroRotator);

	FC_GameUtil::CameraShake();
}

void AC_SkillObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	if (ElapsedTime > LifeTime)
	{
		PlaySound(false);
		PlayFX(false, GetActorLocation());
		Destroy();
	}
}

void AC_SkillObject::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlaySound(false);
	PlayFX(false, SweepResult.Location);
	Destroy();
}

void AC_SkillObject::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AC_SkillObject::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BounceCount > 0)
	{
		BounceCount--;
	}
	else if (BounceCount == 0)
	{
		PlaySound(false);
		PlayFX(false, Hit.Location);
		Destroy();
	}
}