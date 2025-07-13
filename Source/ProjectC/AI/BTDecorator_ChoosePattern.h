
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ChoosePattern.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTC_API UBTDecorator_ChoosePattern : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_ChoosePattern();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere)
	float Percent = 0;
};
