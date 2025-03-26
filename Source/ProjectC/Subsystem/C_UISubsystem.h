
#pragma once

#include "CoreMinimal.h"
#include "ProjectC/UI/C_UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "C_UISubsystem.generated.h"


class UC_HUDWidget;

UCLASS()
class PROJECTC_API UC_UISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UC_UISubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	void CreateHUD();
	
	template<typename T>
	UC_UserWidget* CreateUI(T* ClassType);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<UC_HUDWidget> HUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<UC_HUDWidget> HUDWidget;
	
};

template <typename T>
UC_UserWidget* UC_UISubsystem::CreateUI(T* ClassType)
{
	if (!IsValid(ClassType))
		return nullptr;

	if (UC_UserWidget* CreatedWidget = Cast<UC_UserWidget>(CreateWidget(GetWorld(), ClassType)))
	{
		return CreatedWidget;
	}
	
	return nullptr;
}
