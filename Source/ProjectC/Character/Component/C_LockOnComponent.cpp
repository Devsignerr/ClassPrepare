#include "C_LockOnComponent.h"

#include "GameFramework/Character.h"
#include "ProjectC/Interface/C_CharacterWidgetInterface.h"
#include "ProjectC/Subsystem/C_UISubsystem.h"
#include "ProjectC/UI/C_LockOnWidget.h"

UC_LockOnComponent::UC_LockOnComponent()
{
	TargetDetectRadius = 1200.f;
	TargetDetectAngle = 90.f;
}

void UC_LockOnComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UC_LockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

	Target = InActor;

	if (IC_CharacterWidgetInterface* Character = Cast<IC_CharacterWidgetInterface>(Target))
	{
		Character->OnLocked(true);
	}
}

void UC_LockOnComponent::ClearTarget()
{
	if (IC_CharacterWidgetInterface* Character = Cast<IC_CharacterWidgetInterface>(Target))
	{
		Character->OnLocked(false);
	}
	
	Target = nullptr;
}
