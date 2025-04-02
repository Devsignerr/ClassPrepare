
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsLookingTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTC_API UBTDecorator_IsLookingTarget : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsLookingTarget();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
