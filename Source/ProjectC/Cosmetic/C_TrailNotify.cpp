// Fill out your copyright notice in the Description page of Project Settings.


#include "C_TrailNotify.h"

#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
#include "..\Interface\C_CharacterInterface.h"


void UC_TrailNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp)
	{
		if (IC_CharacterInterface* AttackPawn = Cast<IC_CharacterInterface>(MeshComp->GetOwner()))
		{
			if (bWeaponTrail && AttackPawn->HasWeapon())
			{
				UStaticMeshComponent* StaticMeshComponent = AttackPawn->GetStaticMeshComponent();
				check(StaticMeshComponent);

				TPair<FName, FName> WeaponTraceNames = AttackPawn->GetWeaponTraceNames();

				FVector StartPos = StaticMeshComponent->GetSocketLocation(WeaponTraceNames.Key);
				FVector EndPos = StaticMeshComponent->GetSocketLocation(WeaponTraceNames.Value);

				if (UFXSystemComponent* FXSystemComponent = GetSpawnedEffect(MeshComp))
				{
					FXSystemComponent->SetVectorParameter(TEXT("StartTrail"), StartPos);
					FXSystemComponent->SetVectorParameter(TEXT("EndTrail"), EndPos);
				}
			}
			else if (!bWeaponTrail)
			{
				ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner());
				check(OwnerCharacter);

				USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
				check(SkeletalMeshComponent);

				FVector StartPos = SkeletalMeshComponent->GetSocketLocation(BodyTrailBoneName_Start);
				FVector EndPos = SkeletalMeshComponent->GetSocketLocation(BodyTrailBoneName_End);

				if (UFXSystemComponent* FXSystemComponent = GetSpawnedEffect(MeshComp))
				{
					FXSystemComponent->SetVectorParameter(TEXT("StartTrail"), StartPos);
					FXSystemComponent->SetVectorParameter(TEXT("EndTrail"), EndPos);
				}
			}
		}
	}
}

UFXSystemComponent* UC_TrailNotify::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	UFXSystemComponent* System = Super::SpawnEffect(MeshComp, Animation);
	return System;
}
