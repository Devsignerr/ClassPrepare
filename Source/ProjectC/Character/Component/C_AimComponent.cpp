#include "C_AimComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectC/Data/C_CameraDataAsset.h"
#include "ProjectC/Interface/C_PlayerCharacterInterface.h"
#include "ProjectC/Utils/C_GameUtil.h"

class IC_PlayerCharacterInterface;

UC_AimComponent::UC_AimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_AimComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = CastChecked<ACharacter>(GetOwner());
	CurrentCameraType = EC_CameraType::Normal;

	const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
	USpringArmComponent* SpringArmComponent = Interface->GetSpringArmComponent();
	check(SpringArmComponent);

	UCameraComponent* CameraComponent = Interface->GetCameraComponent();
	check(CameraComponent);

	const FVector TargetOffset = FC_GameUtil::GetCameraData(CurrentCameraType)->SocketOffset;
	const FRotator TargetArmRotation = FC_GameUtil::GetCameraData(CurrentCameraType)->CameraRot;
	const float TargetArmLength = FC_GameUtil::GetCameraData(CurrentCameraType)->TargetArmLength;

	SpringArmComponent->SocketOffset = TargetOffset;
	CameraComponent->SetRelativeRotation(TargetArmRotation);
	SpringArmComponent->TargetArmLength = TargetArmLength;
}

void UC_AimComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCameraBlending)
	{
		const IC_PlayerCharacterInterface* Interface = CastChecked<IC_PlayerCharacterInterface>(GetOwner());
		USpringArmComponent* SpringArmComponent = Interface->GetSpringArmComponent();
		check(SpringArmComponent);

		UCameraComponent* CameraComponent = Interface->GetCameraComponent();
		check(CameraComponent);

		const FVector TargetOffset = FC_GameUtil::GetCameraData(CurrentCameraType)->SocketOffset;
		const FRotator TargetArmRotation = FC_GameUtil::GetCameraData(CurrentCameraType)->CameraRot;
		const float TargetArmLength = FC_GameUtil::GetCameraData(CurrentCameraType)->TargetArmLength;
		
		// 보간 처리
		const FVector NewOffset = FMath::VInterpTo(SpringArmComponent->SocketOffset, TargetOffset, DeltaTime, 30.f);
		const FRotator NewRot = FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetArmRotation, DeltaTime, 30.f);
		const float NewLen = FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetArmLength, DeltaTime, 30.f);

		SpringArmComponent->SocketOffset = NewOffset;
		CameraComponent->SetRelativeRotation(NewRot);
		SpringArmComponent->TargetArmLength = NewLen;
		
		if ((TargetOffset - NewOffset).Length() <= 0.1f)
		{
			bCameraBlending = false;	
		}
	}

	if (CurrentCameraType == EC_CameraType::Aim)
	{
		CalcAimOffset(DeltaTime);
	}
}

void UC_AimComponent::CalcAimOffset(float DeltaTime)
{
	check(OwnerCharacter.Get());

	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator ActorRotation = OwnerCharacter->GetActorRotation();

	FRotator NormalizedDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);

	float NewPitch = FMath::FInterpTo(AimOffsetRotation.Pitch, NormalizedDeltaRotation.Pitch, DeltaTime, 30.f);
	float NewYaw = FMath::FInterpTo(AimOffsetRotation.Yaw, NormalizedDeltaRotation.Yaw, DeltaTime, 30.f);
	
	AimOffsetRotation = FRotator(NewPitch, NewYaw, 0.f);
}

void UC_AimComponent::SwitchCamera(EC_CameraType CameraType)
{
	CurrentCameraType = CameraType;
	bCameraBlending = true;
}
