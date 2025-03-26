#include "C_ActionComponent.h"

UC_ActionComponent::UC_ActionComponent()
{
}

void UC_ActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UC_ActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
