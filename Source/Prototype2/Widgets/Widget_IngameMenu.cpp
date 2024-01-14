


#include "Widget_IngameMenu.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"

void UWidget_IngameMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	HoverButton(bIsButtonHovered, ButtonToPulse, PulseTime, InDeltaTime, FVector2D(1.0f, 1.0f), ButtonScale);

	
}

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

		/* Toggle character movement - stops movement while menu open */
		//if (auto Owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
		//{
		//	Owner->GetCharacterMovement()->ToggleActive();
		//}
	}
}

void UWidget_IngameMenu::DisableMenu()
{
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
