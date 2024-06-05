

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

void UWidget_MainMenu::NativeDestruct()
{
	Super::NativeDestruct();

	ShowLoadingScreen();
}

void UWidget_MainMenu::RemoveLoadingScreen()
{
	UPrototypeGameInstance* GameInstance = GetGameInstance<UPrototypeGameInstance>();
	
	if (!GameInstance)
		return;

	if (!GameInstance->BlackScreenWidget)
		return;
		
	GameInstance->RemoveLoadingScreen(GameInstance->BlackScreenWidget);
}

void UWidget_MainMenu::ShowLoadingScreen()
{
	UPrototypeGameInstance* GameInstance = GetGameInstance<UPrototypeGameInstance>();
	
	if (!GameInstance)
		return;

	if (!GameInstance->BlackScreenWidget)
		return;
	
	GameInstance->ShowLoadingScreen(GameInstance->BlackScreenWidget, 0);
}
