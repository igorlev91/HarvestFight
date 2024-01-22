

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

	/* Set map choice to hidden */
	WBP_MapChoiceBrawl->SetVisibility(ESlateVisibility::Hidden);
	
	/* Add rings to array */
	Rings.Add(P1Ring);
	Rings.Add(P2Ring);
	Rings.Add(P3Ring);
	Rings.Add(P4Ring);
	Rings.Add(P5Ring);
	Rings.Add(P6Ring);

	// Add icons to array
	Icons.Add(P1Icon);
	Icons.Add(P2Icon);
	Icons.Add(P3Icon);
	Icons.Add(P4Icon);
	Icons.Add(P5Icon);
	Icons.Add(P6Icon);

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

			/* Set player icons to correct image based on character and colour */
			if (Icons.Num() > i)
			{
				switch (Player->Details.Character)
				{
				case ECharacters::COW:
					{
						Icons[i]->SetBrushFromTexture(Player->CowTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::PIG:
					{
						Icons[i]->SetBrushFromTexture(Player->PigTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::CHICKEN:
					{
						Icons[i]->SetBrushFromTexture(Player->ChickenTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::DUCK:
					{
						Icons[i]->SetBrushFromTexture(Player->DuckTextures[(int32)Player->Details.Colour]);
						break;
					}
				default:
					{
						UE_LOG(LogTemp, Warning, TEXT("Error: Widget_LobbyPlayerHUDV2: Unable to determine character type"));
						break;
					}
				}
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
		if (GameStateReference->GetGameMode() == 0) // Normal Game mode
		{
			UpdateMapChoice(WBP_MapChoice);
		}
		else // Brawl
		{
			UpdateMapChoice(WBP_MapChoiceBrawl);
		}
		
	}
	
	/* Show timer after map vote */
	if (GameStateReference->ShouldShowMapChoices())
	{
		if (GameStateReference->GetGameMode() == 0) // Normal Game mode
			{
				UpdateMapChoiceTimer(WBP_MapChoice);
			}
		else // Brawl
			{
				UpdateMapChoiceTimer(WBP_MapChoiceBrawl);
			}
		
	}
}

void UWidget_LobbyPlayerHUDV2::UpdateMapChoice(UWidget_MapChoice* _MapChoiceWidget)
{
	_MapChoiceWidget->SetVisibility(ESlateVisibility::Visible); // Turn widget on for map choice

	/* Increasing value of counter for each map */
	_MapChoiceWidget->NormalLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFarm()))); // Increase vote counter for map
	_MapChoiceWidget->WinterLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetWinterFarm()))); // Increase vote counter for map
	_MapChoiceWidget->HoneyLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetHoneyFarm()))); // Increase vote counter for map
	_MapChoiceWidget->FloatingIslandsLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFloatingIslandFarm()))); // Increase vote counter for map

	/* Turning on visibility of maps if value is higher than 0 */
	if (GameStateReference->GetFarm() > 0) // Normal farm
		_MapChoiceWidget->NormalLevelCounter->SetVisibility(ESlateVisibility::Visible); 
	else
		_MapChoiceWidget->NormalLevelCounter->SetVisibility(ESlateVisibility::Hidden);
	
	if (GameStateReference->GetWinterFarm() > 0) // Winter farm
		_MapChoiceWidget->WinterLevelCounter->SetVisibility(ESlateVisibility::Visible); 
	else
		_MapChoiceWidget->WinterLevelCounter->SetVisibility(ESlateVisibility::Hidden);
	
	if (GameStateReference->GetHoneyFarm() > 0) // Honey farm
		_MapChoiceWidget->HoneyLevelCounter->SetVisibility(ESlateVisibility::Visible); 
	else
		_MapChoiceWidget->HoneyLevelCounter->SetVisibility(ESlateVisibility::Hidden);

	if (GameStateReference->GetFloatingIslandFarm() > 0) // Floating islands farm
		_MapChoiceWidget->FloatingIslandsLevelCounter->SetVisibility(ESlateVisibility::Visible); 
	else
		_MapChoiceWidget->FloatingIslandsLevelCounter->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_LobbyPlayerHUDV2::UpdateMapChoiceTimer(UWidget_MapChoice* _MapChoiceWidget)
{
	_MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMapChoiceTotalLengthSeconds())));
	if (GameStateReference->GetMapChoiceTotalLengthSeconds() <= 0)
	{
		_MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString("LOADING LEVEL...")));
	}
}
