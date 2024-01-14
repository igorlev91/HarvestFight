

#include "Widget_LobbyPlayerHUDV2.h"
#include "Widget_MapChoice.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

void UWidget_LobbyPlayerHUDV2::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/* Set gamestate reference */
	if (auto* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}
	
	/* Add rings to array */
	Rings.Add(P1Ring);
	Rings.Add(P2Ring);
	Rings.Add(P3Ring);
	Rings.Add(P4Ring);
	Rings.Add(P5Ring);
	Rings.Add(P6Ring);

	/* Add Player names to array*/
	Names.Add(P1Name);
	Names.Add(P2Name);
	Names.Add(P3Name);
	Names.Add(P4Name);
	Names.Add(P5Name);
	Names.Add(P6Name);

	/* Add Player overlays names to array*/
	PlayerHorizontalBoxes.Add(Player1HorizontalBox);
	PlayerHorizontalBoxes.Add(Player2HorizontalBox);
	PlayerHorizontalBoxes.Add(Player3HorizontalBox);
	PlayerHorizontalBoxes.Add(Player4HorizontalBox);
	PlayerHorizontalBoxes.Add(Player5HorizontalBox);
	PlayerHorizontalBoxes.Add(Player6HorizontalBox);

	/* Add ready images to array */
	ReadyImages.Add(P1ReadyImage);
	ReadyImages.Add(P2ReadyImage);
	ReadyImages.Add(P3ReadyImage);
	ReadyImages.Add(P4ReadyImage);
	ReadyImages.Add(P5ReadyImage);
	ReadyImages.Add(P6ReadyImage);

	LeftBoxBackgroundImage->SetVisibility(ESlateVisibility::Visible);
}

void UWidget_LobbyPlayerHUDV2::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GameStateReference)
	{
		return;
	}
	
	/* Make all player horizontal boxes hidden on start of frame */
	for (int i = 1; i < 6; i++)
	{
		PlayerHorizontalBoxes[i]->SetVisibility(ESlateVisibility::Hidden);
	}

	/* Make player overlay backgrounds hidden on start of frame */
	RightBoxBackgroundImage->SetVisibility(ESlateVisibility::Hidden);
	
	for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_Players[i])
		{
			/* Set ring colours to player colour */
			if (Rings.Num() > i)
			{
				Rings[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));
			}

			/* Set player names to actual names and changing colour to player colour */
			if (Names.Num() > i)
			{
				Names[i]->SetText(FText::FromString(FString(Player->PlayerName)));
				Names[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));
			}

			/* Turn on second background depending on number of players */
			if (i > 2)
			{
				RightBoxBackgroundImage->SetVisibility(ESlateVisibility::Visible);
			}
			
			/* Showing player overlays on HUD as they join lobby */
			if (PlayerHorizontalBoxes.Num() > i)
			{
				/* Make individual overlay visible */
				PlayerHorizontalBoxes[i]->SetVisibility(ESlateVisibility::Visible);
			}

			/* Showing player overlays on HUD as they join lobby */
			if (ReadyImages.Num() > i)
			{
				if (Player->IsReady == true)
					ReadyImages[i]->SetVisibility(ESlateVisibility::Visible);
				else
					ReadyImages[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
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
}
