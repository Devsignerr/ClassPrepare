
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectC/enums.h"
#include "C_SkillObject.generated.h"

class USphereComponent;

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
	
	void PlaySound(bool bSpawn);
	void PlayFX(bool bSpawn, FVector InHitLocation);

public:
	UPROPERTY(EditAnywhere)
	EPC_SkillObjectType SkillObjectType = EPC_SkillObjectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* Collision_Environment = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* TriggerCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY()
	ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> SpawnSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> DeSpawnSound;
	
	UPROPERTY(EditAnywhere)
	float LifeTime = 0.f;
	float ElapsedTime = 0.f;

	float ExplodeRadius = 0.f;

	FVector PreviousLocation = FVector::ZeroVector;
	
	int32 BounceCount = 0;
};
