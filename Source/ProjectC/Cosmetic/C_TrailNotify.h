// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "C_TrailNotify.generated.h"

UCLASS()
class PROJECTC_API UC_TrailNotify : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const override;

	UPROPERTY(EditAnywhere)
	bool bWeaponTrail = false;

	UPROPERTY(EditAnywhere)
	FName BodyTrailBoneName_Start;

	UPROPERTY(EditAnywhere)
	FName BodyTrailBoneName_End;
};
