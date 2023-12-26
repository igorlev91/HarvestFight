#include "Widget_MainMenu.h"

#include "Components/Button.h"
#include "Prototype2/PrototypeGameInstance.h"

void UWidget_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UWidget_MainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstance = Cast<UPrototypeGameInstance>(GetGameInstance());
}
