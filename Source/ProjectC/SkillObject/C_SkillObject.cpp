#include "C_SkillObject.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	ParticleComponent->SetupAttachment(RootComponent);
	
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
}

void AC_SkillObject::PlaySound()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DeSpawnSound, GetActorLocation());
}

void AC_SkillObject::PlayFX(FVector InHitLocation)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DespawnFX, GetActorLocation(), GetActorRotation());
	FC_GameUtil::CameraShake();
}

void AC_SkillObject::ProcessDestroy()
{
	TriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision_Environment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent->Deactivate();
	StaticMeshComponent->SetVisibility(false);

	PlaySound();
	PlayFX(GetActorLocation());

	NiagaraComponent->SetBoolParameter(TEXT("Destroyed"), true);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		Destroy();
	}), 1.f, false);
}

void AC_SkillObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	if (ElapsedTime > LifeTime)
	{
		ProcessDestroy();
	}
}

void AC_SkillObject::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ProcessDestroy();
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
		ProcessDestroy();
	}
}