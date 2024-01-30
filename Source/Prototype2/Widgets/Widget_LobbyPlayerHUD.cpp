

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
	ReadyButton->SetVisibility(ESlateVisibility::Visible);
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
		WBP_MapChoice->SetVisibility(ESlateVisibility::Visible); // Turn widget on for map choice

		/* Increasing value of counter for each map */
		WBP_MapChoice->NormalLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFarm()))); // Increase vote counter for map
		WBP_MapChoice->WinterLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetWinterFarm()))); // Increase vote counter for map
		WBP_MapChoice->HoneyLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetHoneyFarm()))); // Increase vote counter for map

		/* Turning on visibility of maps if value is higher than 0 */
		if (GameStateReference->GetFarm() > 0) // Normal farm
		{
			WBP_MapChoice->NormalLevelCounter->SetVisibility(ESlateVisibility::Visible); 
		}
		else
		{
			WBP_MapChoice->NormalLevelCounter->SetVisibility(ESlateVisibility::Hidden);
		}
		if (GameStateReference->GetWinterFarm() > 0) // Winter farm
		{
			WBP_MapChoice->WinterLevelCounter->SetVisibility(ESlateVisibility::Visible); 
		}
		else
		{
			WBP_MapChoice->WinterLevelCounter->SetVisibility(ESlateVisibility::Hidden);
		}
		if (GameStateReference->GetHoneyFarm() > 0) // Honey farm
		{
			WBP_MapChoice->HoneyLevelCounter->SetVisibility(ESlateVisibility::Visible); 
		}
		else
		{
			WBP_MapChoice->HoneyLevelCounter->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	/* Show timer after map vote */
	WBP_MapChoice->MapChoiceTimer->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMapChoiceTotalLengthSeconds())));
	if (GameStateReference->GetMapChoiceTotalLengthSeconds() <= 0)
	{
		WBP_MapChoice->MapChoiceTimer->SetText(FText::FromString(FString("LOADING LEVEL...")));
	}
	
	HoverButton(bIsButtonHovered, ButtonToPulse, PulseTime, InDeltaTime, FVector2D(1.0f, 1.0f), ButtonScale);
}

void UWidget_LobbyPlayerHUD::SetReady()
{
	if (auto PlayerState = Cast<ALobbyPlayerState>(GetOwningPlayerState()))
	{
		auto PlayerID = PlayerState->Player_ID;
		
		if (GameStateReference->Server_Players.Num() >= PlayerID)
		{
			if (auto PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
			{
				PlayerController->SetIsReady(PlayerID, true);
			}

			ReadyButton->SetVisibility(ESlateVisibility::Hidden);
			CancelButton->SetVisibility(ESlateVisibility::Visible);
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

