
#include "C_UISubsystem.h"
#include "../UI/C_HUDWidget.h"

UC_UISubsystem::UC_UISubsystem()
{
	static ConstructorHelpers::FClassFinder<UC_HUDWidget> HUDWidgetAsset(TEXT("/Game/UI/WBP_HUD.WBP_HUD_C"));
	if (HUDWidgetAsset.Succeeded())
	{
		HUDWidgetClass = HUDWidgetAsset.Class;
	}
}

void UC_UISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UC_UISubsystem::CreateHUD()
{
	if (UC_HUDWidget* Widget = Cast<UC_HUDWidget>(CreateUI(HUDWidgetClass.Get())))
	{
		Widget->AddToViewport();
		HUDWidget = Widget;
	}
}
