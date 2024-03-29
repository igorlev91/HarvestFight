

#include "Widget_LobbyPlayerHUDV2.h"

#include "Widget_LobbyCharacterSelection.h"
#include "Widget_MapChoice.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "SlateCore/Public/Fonts/SlateFontInfo.h"

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
    Overlays.Add(OverlayPlayer1);
    Overlays.Add(OverlayPlayer2);
    Overlays.Add(OverlayPlayer3);
    Overlays.Add(OverlayPlayer4);
    Overlays.Add(OverlayPlayer5);
    Overlays.Add(OverlayPlayer6);
	
	/* Add rings to array */
	Rings.Add(P1Ring);
	Rings.Add(P2Ring);
	Rings.Add(P3Ring);
	Rings.Add(P4Ring);
	Rings.Add(P5Ring);
	Rings.Add(P6Ring);

	/* Add backgrounds to array */
	Backgrounds.Add(P1Background);
	Backgrounds.Add(P2Background);
	Backgrounds.Add(P3Background);
	Backgrounds.Add(P4Background);
	Backgrounds.Add(P5Background);
	Backgrounds.Add(P6Background);

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

	/* Add ready images to array */
	ReadyImages.Add(P1ReadyImage);
	ReadyImages.Add(P2ReadyImage);
	ReadyImages.Add(P3ReadyImage);
	ReadyImages.Add(P4ReadyImage);
	ReadyImages.Add(P5ReadyImage);
	ReadyImages.Add(P6ReadyImage);

	BackgroundImageT1->SetOpacity(0.0f);
	BackgroundImageT2->SetOpacity(0.0f);
}

void UWidget_LobbyPlayerHUDV2::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GameStateReference)
	{
		return;
	}

	InitTeams();
	UpdateTeams();

	//UE_LOG(LogTemp, Warning, TEXT("Team 1 size: %d"), GameStateReference->Server_TeamOne.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Team 2 size: %d"), GameStateReference->Server_TeamTwo.Num());
	
	/* Make all player overlays hidden on start of frame */
	for (int i = 1; i < Overlays.Num(); i++)
	{
		Overlays[i]->SetVisibility(ESlateVisibility::Hidden);
	}
	
	for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_Players[i])
		{
			/* Set ring colours to player colour */
			if (Rings.Num() > i)
			{
				Rings[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));
			}

			/* Set ring colours to player colour */
			if (Backgrounds.Num() > i)
			{
				Backgrounds[i]->SetColorAndOpacity(FLinearColor(
					Player->Details.PureToneColour.X,
					Player->Details.PureToneColour.Y,
					Player->Details.PureToneColour.Z,
					0.7f));
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

			/* Set player names to actual names */
			if (Names.Num() > i)
			{
				Names[i]->SetText(FText::FromString(FString(Player->PlayerName)));
			}

			/* Turn on overlays */
			/* Showing player overlays on HUD as they join lobby */
			if (Overlays.Num() > i)
			{
				/* Make individual overlay visible */
				Overlays[i]->SetVisibility(ESlateVisibility::Visible);
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
		else if (GameStateReference->GetGameMode() == 1) // Brawl
		{
			UpdateMapChoice(WBP_MapChoiceBrawl);
		}
		else
		{
			UpdateMapChoice(WBP_MapChoiceBlitz);
		}
	}
	
	/* Show timer after map vote */
	if (GameStateReference->ShouldShowMapChoices())
	{
		if (GameStateReference->GetGameMode() == 0) // Normal Game mode
		{
			UpdateMapChoiceTimer(WBP_MapChoice);
		}
		else if (GameStateReference->GetGameMode() == 1)// Brawl
		{
			UpdateMapChoiceTimer(WBP_MapChoiceBrawl);
		}
		else // Blitz
		{
			UpdateMapChoiceTimer(WBP_MapChoiceBlitz);
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

	if (GameStateReference->GetFloatingIslandFarm() > 0) // Clockwork farm
		_MapChoiceWidget->ClockworkLevelCounter->SetVisibility(ESlateVisibility::Visible); 
	else
		_MapChoiceWidget->ClockworkLevelCounter->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_LobbyPlayerHUDV2::UpdateMapChoiceTimer(UWidget_MapChoice* _MapChoiceWidget)
{
	_MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMapChoiceTotalLengthSeconds())));
	if (GameStateReference->GetMapChoiceTotalLengthSeconds() <= 0)
	{
		_MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString("LOADING LEVEL...")));
	}
}

void UWidget_LobbyPlayerHUDV2::SetOwningController(int32 _PlayerID,APrototype2PlayerController* _Owner)
{
	WBP_LobbyCharacterSelection->SetOwningController(_PlayerID, _Owner);
}

void UWidget_LobbyPlayerHUDV2::InitTeams()
{
	if (bTeams && !WBP_LobbyCharacterSelection->bTeams)
	{
		TeamAText->SetVisibility(ESlateVisibility::Visible);
		TeamBText->SetVisibility(ESlateVisibility::Visible);
		
		WBP_LobbyCharacterSelection->Button_LeftColour->SetVisibility(ESlateVisibility::Hidden);
		WBP_LobbyCharacterSelection->Button_RightColour->SetVisibility(ESlateVisibility::Hidden);
		
		WBP_LobbyCharacterSelection->bTeams = bTeams;
		if (ColourData)
		{
			TeamAText->SetColorAndOpacity(ColourData->PureColours[(int32)GameStateReference->TeamOneColour]);
			TeamBText->SetColorAndOpacity(ColourData->PureColours[(int32)GameStateReference->TeamTwoColour]);
		}

		BackgroundImageT1->SetOpacity(0.4f);
		BackgroundImageT2->SetOpacity(0.4f);

		LeftTeamOverlay->SetRenderTranslation(FVector2d(0, -100.0f));
		RightTeamOverlay->SetRenderTranslation(FVector2d(0, -100.0f));
		
		if (Overlays.Num() > 0)
		{
			for (int i = 0; i < 6; i++)
			{
				Overlays[i]->RemoveFromParent();
				VerticalBoxLeft->InvalidateLayoutAndVolatility();
				VerticalBoxRight->InvalidateLayoutAndVolatility();
			}
		}
	}
}

void UWidget_LobbyPlayerHUDV2::UpdateTeams()
{
	if (!bTeams)
		return;

	/* Set outline colour to white if black team */
	FSlateFontInfo FontInfo{};
	FontInfo = TeamAText->GetFont();
	FontInfo.OutlineSettings = TeamAText->GetFont().OutlineSettings;
	FontInfo.OutlineSettings.OutlineColor = FLinearColor::White;
	TeamAText->SetText(FText::FromString(GameStateReference->TeamOneName));
	TeamBText->SetText(FText::FromString(GameStateReference->TeamTwoName));
	
	if (GameStateReference->TeamOneColour == EColours::BLACK)
		TeamAText->SetFont(FontInfo);
	if (GameStateReference->TeamTwoColour == EColours::BLACK)
		TeamBText->SetFont(FontInfo);
	
	/* Update team 1 positions in vertical box */
	for (int i = 0; i < GameStateReference->Server_TeamOne.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_TeamOne[i])
		{
			switch (Player->Player_ID)
			{
			case 0:
				{
					if (!VerticalBoxLeft->HasChild(OverlayPlayer1))
						VerticalBoxLeft->AddChild(OverlayPlayer1);
					break;
				}
			case 1:
				{
					if (!VerticalBoxLeft->HasChild(OverlayPlayer2))
						VerticalBoxLeft->AddChild(OverlayPlayer2);
					break;
				}
			case 2:
				{
					if (!VerticalBoxLeft->HasChild(OverlayPlayer3))
						VerticalBoxLeft->AddChild(OverlayPlayer3);
					break;
				}
			case 3:
				{
					if (!VerticalBoxLeft->HasChild(OverlayPlayer4))
						VerticalBoxLeft->AddChild(OverlayPlayer4);
					break;
				}
			case 4:
				{
					if (!VerticalBoxLeft->HasChild(OverlayPlayer5))
						VerticalBoxLeft->AddChild(OverlayPlayer5);
					break;
				}
			case 5:
				{
					if (!VerticalBoxLeft->HasChild(OverlayPlayer6))
						VerticalBoxLeft->AddChild(OverlayPlayer6);
					break;
				}
			case 6:
				{
					// Do nothing
					break;
				}
			}
		}
	}

	/* Adding padding */
	if (VerticalBoxLeft->GetSlots().Num() > 1)
	{
		for (int i = 1; i < VerticalBoxLeft->GetSlots().Num(); i++)
		{
			UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(VerticalBoxLeft->GetSlots()[i]);

			if (VerticalSlot->Content->GetVisibility() == ESlateVisibility::Hidden)
			{
				VerticalSlot->Content->RemoveFromParent();
				//VerticalBoxLeft->RemoveChildAt(i);
				i--;
			}
			else
			{
				VerticalSlot->SetPadding(FMargin(0,30,0,0));
			}
		}
	}
	
	VerticalBoxLeft->InvalidateLayoutAndVolatility();

	/* Update team 2 positions in vertical box */
	for (int i = 0; i < GameStateReference->Server_TeamTwo.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_TeamTwo[i])
		{
			switch (Player->Player_ID)
			{
			case 0:
				{
					if (!VerticalBoxRight->HasChild(OverlayPlayer1))
					{
						VerticalBoxRight->AddChild(OverlayPlayer1);
					}
					break;
				}
			case 1:
				{
					if (!VerticalBoxRight->HasChild(OverlayPlayer2))
					{
						VerticalBoxRight->AddChild(OverlayPlayer2);
					}
					break;
				}
			case 2:
				{
					if (!VerticalBoxRight->HasChild(OverlayPlayer3))
					{
						VerticalBoxRight->AddChild(OverlayPlayer3);
					}
					break;
				}
			case 3:
				{
					if (!VerticalBoxRight->HasChild(OverlayPlayer4))
						VerticalBoxRight->AddChild(OverlayPlayer4);
					break;
				}
			case 4:
				{
					if (!VerticalBoxRight->HasChild(OverlayPlayer5))
						VerticalBoxRight->AddChild(OverlayPlayer5);
					break;
				}
			case 5:
				{
					if (!VerticalBoxRight->HasChild(OverlayPlayer6))
						VerticalBoxRight->AddChild(OverlayPlayer6);
					break;
				}
			case 6:
				{
					// Do nothing
					break;
				}
			}
		}
	}

	/* Adding padding */
	if (VerticalBoxRight->GetSlots().Num() > 1)
	{
		for (int i = 1; i < VerticalBoxRight->GetSlots().Num(); i++)
		{
			UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(VerticalBoxRight->GetSlots()[i]);

			if (VerticalSlot->Content->GetVisibility() == ESlateVisibility::Hidden)
			{
				VerticalBoxRight->RemoveChildAt(i);
				i--;
			}
			else
			{
				VerticalSlot->SetPadding(FMargin(0,30,0,0));
			}
		}
	}
	
	VerticalBoxRight->InvalidateLayoutAndVolatility();
}

void UWidget_LobbyPlayerHUDV2::Client_SetOwningController_Implementation(int32 _PlayerID,
                                                                         APrototype2PlayerController* _Owner)
{
	WBP_LobbyCharacterSelection->SetOwningController(_PlayerID, _Owner);
}
