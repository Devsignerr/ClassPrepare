#include "C_LockOnComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectC/Interface/C_CharacterWidgetInterface.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"

UC_LockOnComponent::UC_LockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	TargetDetectRadius = 1200.f;
	TargetDetectAngle = 90.f;
}

void UC_LockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsLockOnMode())
	{
		ACharacter* Owner = Cast<ACharacter>(GetOwner());
		check(Owner);

		APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
		check(PlayerController);

		if (LockedTarget.Get())
		{
			const FVector LockTargetLocation = LockedTarget->GetActorLocation();
			const FVector OwnerLocation = GetOwner()->GetActorLocation();

			const FRotator CurrentRot = PlayerController->GetControlRotation();
			const FRotator TargetRot = (LockTargetLocation - OwnerLocation).Rotation();

			const FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 10.f);
			
			PlayerController->SetControlRotation(NewRot);
		}
	}
}

void UC_LockOnComponent::LockOn()
{
	SetLockOnMode(!IsLockOnMode());
}

APawn* UC_LockOnComponent::FindTarget()
{
	UWorld* CurrentWorld = GetWorld();
	check(CurrentWorld);
	
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	check(Owner);

	APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
	check(PlayerController);

	FVector CameraForward = PlayerController->GetControlRotation().Vector();
	CameraForward.Z = 0.f;
	
	FVector OwnerLocation = Owner->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
			
	TArray<FOverlapResult> OverlapResult;
	CurrentWorld->OverlapMultiByChannel(OverlapResult, OwnerLocation, FQuat::Identity,
		ECC_GameTraceChannel2 , FCollisionShape::MakeSphere(TargetDetectRadius), QueryParams);

	APawn* FoundTarget = nullptr;
	float BestAngle = INT_MAX;

	for (FOverlapResult& Result : OverlapResult)
	{
		APawn* ResultPawn = Cast<APawn>(Result.GetActor());
		if (!ResultPawn)
			continue;
		
		FVector ToTargetDir = (Result.GetActor()->GetActorLocation() - OwnerLocation).GetSafeNormal();
		float OffsetAngle = FMath::RadiansToDegrees(FMath::Acos(ToTargetDir.Dot(CameraForward)));
		if (OffsetAngle < TargetDetectAngle)
		{
			if (OffsetAngle < BestAngle)
			{
				FoundTarget = ResultPawn;
				BestAngle = OffsetAngle;
			}
		}
	}
	
	return FoundTarget;
}

void UC_LockOnComponent::LockTarget(APawn* InActor)
{
	if (!InActor)
		return;

	LockedTarget = InActor;

	if (IC_CharacterWidgetInterface* Character = Cast<IC_CharacterWidgetInterface>(LockedTarget))
	{
		Character->OnLocked(true);
	}
}

void UC_LockOnComponent::ClearTarget()
{
	if (IC_CharacterWidgetInterface* Character = Cast<IC_CharacterWidgetInterface>(LockedTarget))
	{
		Character->OnLocked(false);
	}
	
	LockedTarget = nullptr;
}

void UC_LockOnComponent::SetLockOnMode(bool bEnable)
{
	bLockOnMode = bEnable;
	
	if (bEnable)
	{
		if (APawn* Target = FindTarget())
		{
			LockTarget(Target);
		}
		else
		{
			bLockOnMode = false;
		}
	}
	
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	check(OwnerCharacter);

	if (!bLockOnMode)
	{
		ClearTarget();
		
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}
