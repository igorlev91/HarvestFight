


#include "Widget_IngameMenu.h"
#include "Kismet/GameplayStatics.h"

void UWidget_IngameMenu::ToggleMenu()
{
	if (auto* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (GetVisibility() == ESlateVisibility::Hidden)
		{
			SetVisibility(ESlateVisibility::Visible);

			Controller->SetInputMode(FInputModeGameAndUI{});
			Controller->bShowMouseCursor = true;
		}
		else
		{
			Controller->bShowMouseCursor = false;
			Controller->SetInputMode(FInputModeGameOnly{});
			
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWidget_IngameMenu::DisableMenu()
{
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
