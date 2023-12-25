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
	//Host->OnPressed.AddDynamic(this, &UWidget_MainMenu::OnCreateServerPreessed);
	//Join->OnPressed.AddDynamic(this, &UWidget_MainMenu::OnJoinServerPreessed);
	
}

void UWidget_MainMenu::OnCreateServerPreessed()
{
	GameInstance->CreateServer();
}

void UWidget_MainMenu::OnJoinServerPreessed()
{
	GameInstance->JoinServer();
}
