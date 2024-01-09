

#include "Widget_LobbyPlayerHUD.h"

#include <string>

#include "Widget_MapChoice.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

void UWidget_LobbyPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}

	// Make cancel hidden
	CancelButton->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_LobbyPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GameStateReference)
	{
		return;
	}
	
	/* Showing map choice widget */
	if (GameStateReference->ShouldShowMapChoices())
	{
		MapChoiceWidget->SetVisibility(ESlateVisibility::Visible); // Turn widget on for map choice

		/* Increasing value of counter for each map */
		MapChoiceWidget->NormalLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFarm()))); // Increase vote counter for map
		MapChoiceWidget->WinterLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetWinterFarm()))); // Increase vote counter for map
		MapChoiceWidget->HoneyLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetHoneyFarm()))); // Increase vote counter for map

		/* Turning on visibility of maps if value is higher than 0 */
		if (GameStateReference->GetFarm() > 0) // Normal farm
		{
			MapChoiceWidget->NormalLevelCounter->SetVisibility(ESlateVisibility::Visible); 
		}
		else
		{
			MapChoiceWidget->NormalLevelCounter->SetVisibility(ESlateVisibility::Hidden);
		}
		if (GameStateReference->GetWinterFarm() > 0) // Winter farm
		{
			MapChoiceWidget->WinterLevelCounter->SetVisibility(ESlateVisibility::Visible); 
		}
		else
		{
			MapChoiceWidget->WinterLevelCounter->SetVisibility(ESlateVisibility::Hidden);
		}
		if (GameStateReference->GetHoneyFarm() > 0) // Honey farm
		{
			MapChoiceWidget->HoneyLevelCounter->SetVisibility(ESlateVisibility::Visible); 
		}
		else
		{
			MapChoiceWidget->HoneyLevelCounter->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	/* Show timer after map vote */
	MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMapChoiceTotalLengthSeconds())));
	if (GameStateReference->GetMapChoiceTotalLengthSeconds() <= 0)
	{
		MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString("LOADING LEVEL...")));
	}
}

void UWidget_LobbyPlayerHUD::SetReady()
{
	if (auto* PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		auto PlayerID = PlayerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
		
		if (GameStateReference->Server_Players.Num() >= PlayerID)
		{
			if (auto* playerState = Cast<ALobbyPlayerState>(GameStateReference->Server_Players[PlayerID]))
			{
				PlayerController->SetIsReady(PlayerID, true);

				ReadyButton->SetVisibility(ESlateVisibility::Hidden);
				CancelButton->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UWidget_LobbyPlayerHUD::SetCancel()
{
	if (auto* PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		auto PlayerID = PlayerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
		
		if (GameStateReference->Server_Players.Num() >= PlayerID)
		{
			if (auto* playerState = Cast<ALobbyPlayerState>(GameStateReference->Server_Players[PlayerID]))
			{
				PlayerController->SetIsReady(PlayerID, false);

				ReadyButton->SetVisibility(ESlateVisibility::Visible);
				CancelButton->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

