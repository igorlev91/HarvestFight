

#include "Widget_MainMenu.h"
#include "Components/Button.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"

void UWidget_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidget_MainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstanceReference = Cast<UPrototypeGameInstance>(GetGameInstance());
	
}

void UWidget_MainMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
