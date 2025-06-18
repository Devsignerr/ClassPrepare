
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectC/enums.h"
#include "C_SkillObject.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UNiagaraComponent;

UCLASS()
class PROJECTC_API AC_SkillObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AC_SkillObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void PlaySound();
	void PlayFX(FVector InHitLocation);

	void ProcessDestroy();

public:
	UPROPERTY(EditAnywhere)
	EC_SkillObjectType SkillObjectType = EC_SkillObjectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NiagaraComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* ParticleComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* Collision_Environment = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* TriggerCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY()
	ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> DeSpawnSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> DespawnFX;
	
	UPROPERTY(EditAnywhere)
	float LifeTime = 0.f;
	float ElapsedTime = 0.f;

	float ExplodeRadius = 0.f;

	FVector PreviousLocation = FVector::ZeroVector;
	
	int32 BounceCount = 0;
};
