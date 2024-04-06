

#include "Widget_LobbyPlayerHUDV2.h"

#include "Widget_LobbyCharacterSelection.h"
#include "Widget_MapChoice.h"
#include "Widget_OptionSelector.h"
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

void UWidget_LobbyPlayerHUDV2::NativePreConstruct()
{
	Super::NativePreConstruct();

	GameSpeed = MEDIUM_GAME;
	DefaultGameSpeed = EGameSpeed::MEDIUM_GAME;
	TempGameSpeed = MEDIUM_GAME;

	/* Host Options Setup */

	/* Set game mode control */
	GameMode_Control->OptionText->SetText(FText::FromString("Game Mode"));
	UpdateGameModeText();
	
	/* Set game speed control */
	GameLength_Control->OptionText->SetText(FText::FromString("Game Speed"));
	UpdateGameSpeedText();

	/* Set stealing control */
	Stealing_Control->OptionText->SetText(FText::FromString("Stealing (from plots)"));
	UpdateStealingText();

	/* Set fertiliser control */
	Fertiliser_Control->OptionText->SetText(FText::FromString("Fertiliser Spawn (if available)"));
	UpdateFertiliserText();

	/* Set cement control */
	Cement_Control->OptionText->SetText(FText::FromString("Cement Spawn (if available)"));
	UpdateCementText();
}

void UWidget_LobbyPlayerHUDV2::NativeConstruct()
{
	Super::NativeConstruct();

	/* Extra Settings Setup */

	/* Game Mode control buttons */
	if (GameMode_Control)
	{
		GameMode_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnGameModeControlLeftButtonPressed);
		GameMode_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnGameModeControlRightButtonPressed);
	}
	
	/* Game speed control buttons */
	if (GameLength_Control)
	{
		GameLength_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnGameSpeedControlLeftButtonPressed);
		GameLength_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnGameSpeedControlRightButtonPressed);
	}

	/* Stealing control buttons */
	if (Stealing_Control)
	{
		Stealing_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnStealingControlButtonPressed);
		Stealing_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnStealingControlButtonPressed);
	}

	/* Fertiliser control buttons */
	if (Fertiliser_Control)
	{
		Fertiliser_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnFertiliserControlButtonPressed);
		Fertiliser_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnFertiliserControlButtonPressed);
	}
	
	/* Cement control buttons */
	if (Cement_Control)
	{
		Cement_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnCementControlButtonPressed);
		Cement_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_LobbyPlayerHUDV2::OnCementControlButtonPressed);
	}
}

void UWidget_LobbyPlayerHUDV2::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/* Set gamestate reference */
	if (auto* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}

	/* Set map choice to hidden */
	WBP_MapChoice->SetVisibility(ESlateVisibility::Hidden);
	WBP_MapChoiceBrawl->SetVisibility(ESlateVisibility::Hidden);
	WBP_MapChoiceBlitz->SetVisibility(ESlateVisibility::Hidden);

	// Make cancel hidden
	ReadyButton->SetVisibility(ESlateVisibility::Visible);
	CancelButton->SetVisibility(ESlateVisibility::Hidden);

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

	/* Add you overlays to array */
	YouOverlays.Add(P1YouOverlay);
	YouOverlays.Add(P2YouOverlay);
	YouOverlays.Add(P3YouOverlay);
	YouOverlays.Add(P4YouOverlay);
	YouOverlays.Add(P5YouOverlay);
	YouOverlays.Add(P6YouOverlay);

	/* Add you overlays to array */
	KickOverlays.Add(P1KickOverlay);
	KickOverlays.Add(P2KickOverlay);
	KickOverlays.Add(P3KickOverlay);
	KickOverlays.Add(P4KickOverlay);
	KickOverlays.Add(P5KickOverlay);
	KickOverlays.Add(P6KickOverlay);
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
			
			if (GetOwningPlayerState() == Player)
			{
				YouOverlays[i]->SetVisibility(ESlateVisibility::Visible);
			}
			if (GetOwningPlayerState()->HasAuthority())
			{
				if (GetOwningPlayerState() != Player)
					KickOverlays[i]->SetVisibility(ESlateVisibility::Visible);
			}
			
			
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

void UWidget_LobbyPlayerHUDV2::SetReady()
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

void UWidget_LobbyPlayerHUDV2::SetCancel()
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

void UWidget_LobbyPlayerHUDV2::UpdateMapChoice(UWidget_MapChoice* _MapChoiceWidget)
{
	_MapChoiceWidget->SetVisibility(ESlateVisibility::Visible); // Turn widget on for map choice

	/* Increasing value of counter for each map */
	_MapChoiceWidget->NormalLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFarm()))); // Increase vote counter for map
	_MapChoiceWidget->WinterLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetWinterFarm()))); // Increase vote counter for map
	_MapChoiceWidget->HoneyLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetHoneyFarm()))); // Increase vote counter for map
	_MapChoiceWidget->FloatingIslandsLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFloatingIslandFarm()))); // Increase vote counter for map
	_MapChoiceWidget->ClockworkLevelCounter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFloatingIslandFarm()))); // Increase vote counter for map

	/* Turning on visibility of map counters if value is higher than 0 */
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

	if (GameStateReference->GetClockworkFarm() > 0) // Clockwork farm
		_MapChoiceWidget->ClockworkLevelCounter->SetVisibility(ESlateVisibility::Visible); 
	else
		_MapChoiceWidget->ClockworkLevelCounter->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_LobbyPlayerHUDV2::UpdateMapChoiceTimer(UWidget_MapChoice* _MapChoiceWidget)
{
	_MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMapChoiceTotalLengthSeconds())));
	if (GameStateReference->GetMapChoiceTotalLengthSeconds() <= 0)
	{
		_MapChoiceWidget->LoadingPageFake->SetVisibility(ESlateVisibility::Visible);
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

void UWidget_LobbyPlayerHUDV2::ResetDefaults()
{
	TempGameSpeed = DefaultGameSpeed;
	UpdateGameSpeedText();
	SetGameSpeedControl();
	TempStealingSetting = DefaultStealingSetting;
	UpdateStealingText();
	SetStealingControl();
	TempFertiliserSetting = DefaultFertiliserSetting;
	UpdateFertiliserText();
	SetFertiliserControl();
	TempCementSetting = DefaultCementSetting;
	UpdateCementText();
	SetCementControl();
}

void UWidget_LobbyPlayerHUDV2::ResetSetting()
{
	TempGameSpeed = GameSpeed;
	UpdateGameSpeedText();
	SetGameSpeedControl();
	TempStealingSetting = StealingSetting;
	UpdateStealingText();
	SetStealingControl();
	TempFertiliserSetting = FertiliserSetting;
	UpdateFertiliserText();
	SetFertiliserControl();
	TempCementSetting = CementSetting;
	UpdateCementText();
	SetCementControl();
}

void UWidget_LobbyPlayerHUDV2::ConfirmSetting()
{
	SetGameModeControl();
	SetStealingControl();
	SetFertiliserControl();
	SetCementControl();
}

void UWidget_LobbyPlayerHUDV2::OnGameModeControlLeftButtonPressed()
{
	switch (TempHHGameMode)
	{
	case 0:
		{
			TempHHGameMode = 2;
			break;
		}
	case 1:
		{
			TempHHGameMode = 0;
			break;
		}
	case 2:
		{
			TempHHGameMode = 1;
			break;
		}
	default:
		{
			TempHHGameMode = 0;
			break;
		}
	}

	UpdateGameModeText();
}

void UWidget_LobbyPlayerHUDV2::OnGameModeControlRightButtonPressed()
{
	switch (TempHHGameMode)
	{
	case 0:
		{
			TempHHGameMode = 1;
			break;
		}
	case 1:
		{
			TempHHGameMode = 2;
			break;
		}
	case 2:
		{
			TempHHGameMode = 0;
			break;
		}
	default:
		{
			TempHHGameMode = 0;
			break;
		}
	}

	UpdateGameModeText();
}

void UWidget_LobbyPlayerHUDV2::UpdateGameModeText()
{
	switch (TempHHGameMode)
	{
	case 0:
		{
			GameMode_Control->OptionValueText->SetText(FText::FromString("Classic"));
			break;
		}
	case 1:
		{
			GameMode_Control->OptionValueText->SetText(FText::FromString("Brawl"));
			break;
		}
	case 2:
		{
			GameMode_Control->OptionValueText->SetText(FText::FromString("Blitz"));
			break;
		}
	default:
		{
			GameMode_Control->OptionValueText->SetText(FText::FromString("Classic"));
			break;
		}
	}
}

void UWidget_LobbyPlayerHUDV2::SetGameModeControl()
{
	HHGameMode = TempHHGameMode;
	
	if (!GameStateReference)
		return;

	GameStateReference->SetGameMode(HHGameMode); // Set game mode in gamestate so that transfer to correct map
}

void UWidget_LobbyPlayerHUDV2::OnGameSpeedControlLeftButtonPressed()
{
	switch (TempGameSpeed)
	{
	case EGameSpeed::SHORT_GAME:
		{
			TempGameSpeed = EGameSpeed::LONG_GAME;
			break;
		}
	case EGameSpeed::MEDIUM_GAME:
		{
			TempGameSpeed = EGameSpeed::SHORT_GAME;
			break;
		}
	case EGameSpeed::LONG_GAME:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	default:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	}

	UpdateGameSpeedText();
}

void UWidget_LobbyPlayerHUDV2::OnGameSpeedControlRightButtonPressed()
{
	switch (TempGameSpeed)
	{
	case EGameSpeed::SHORT_GAME:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	case EGameSpeed::MEDIUM_GAME:
		{
			TempGameSpeed = EGameSpeed::LONG_GAME;
			break;
		}
	case EGameSpeed::LONG_GAME:
		{
			TempGameSpeed = EGameSpeed::SHORT_GAME;
			break;
		}
	default:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	}
	UpdateGameSpeedText();
}

void UWidget_LobbyPlayerHUDV2::UpdateGameSpeedText()
{
	switch (TempGameSpeed)
	{
	case EGameSpeed::SHORT_GAME:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Short"));
			break;
		}
	case EGameSpeed::MEDIUM_GAME:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case EGameSpeed::LONG_GAME:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Long"));
			break;
		}
	default:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	}
}

void UWidget_LobbyPlayerHUDV2::SetGameSpeedControl()
{
	GameSpeed = TempGameSpeed;
}

void UWidget_LobbyPlayerHUDV2::OnStealingControlButtonPressed()
{
	TempStealingSetting = !TempStealingSetting;
	UpdateStealingText();
}

void UWidget_LobbyPlayerHUDV2::UpdateStealingText()
{
	if (TempStealingSetting)
		Stealing_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		Stealing_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_LobbyPlayerHUDV2::SetStealingControl()
{
	StealingSetting = TempStealingSetting;
}

void UWidget_LobbyPlayerHUDV2::OnFertiliserControlButtonPressed()
{
	TempFertiliserSetting = !TempFertiliserSetting;
	UpdateFertiliserText();
}

void UWidget_LobbyPlayerHUDV2::UpdateFertiliserText()
{
	if (TempFertiliserSetting)
		Fertiliser_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		Fertiliser_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_LobbyPlayerHUDV2::SetFertiliserControl()
{
	FertiliserSetting = TempFertiliserSetting;
}

void UWidget_LobbyPlayerHUDV2::OnCementControlButtonPressed()
{
	TempCementSetting = !TempCementSetting;
	UpdateCementText();
}

void UWidget_LobbyPlayerHUDV2::UpdateCementText()
{
	if (TempCementSetting)
		Cement_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		Cement_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_LobbyPlayerHUDV2::SetCementControl()
{
	CementSetting = TempCementSetting;
}

void UWidget_LobbyPlayerHUDV2::Client_SetOwningController_Implementation(int32 _PlayerID,
                                                                         APrototype2PlayerController* _Owner)
{
	WBP_LobbyCharacterSelection->SetOwningController(_PlayerID, _Owner);
}
