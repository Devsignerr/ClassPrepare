// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "C_NotifyFireProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTC_API UC_NotifyFireProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere)
	uint32 SKillObjectId = 0;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FireFX = nullptr; 

	UPROPERTY(EditAnywhere)
	FName FireBoneName;
};
