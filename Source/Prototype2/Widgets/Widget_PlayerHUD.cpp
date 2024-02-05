

#include "Widget_PlayerHUD.h"

#include "Widget_EmoteRadial.h"
#include "Widget_EndgameMenu.h"
#include "Widget_GameOptions.h"
#include "Widget_IngameMenu.h"
#include "Widget_StartAndEndMenu.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Widgets/SOverlay.h"
#include "Layout/Margin.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"

void UWidget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	/* Get gamestate and set to reference variable */
	if (auto* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
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

	/* Add overlays to array */
	Overlays.Add(OverlayPlayer1);
	Overlays.Add(OverlayPlayer2);
	Overlays.Add(OverlayPlayer3);
	Overlays.Add(OverlayPlayer4);
	Overlays.Add(OverlayPlayer5);
	Overlays.Add(OverlayPlayer6);

	// Add icons to array
	Icons.Add(P1Icon);
	Icons.Add(P2Icon);
	Icons.Add(P3Icon);
	Icons.Add(P4Icon);
	Icons.Add(P5Icon);
	Icons.Add(P6Icon);

	// Add player names to array
	PlayerNames.Add(P1Name);
	PlayerNames.Add(P2Name);
	PlayerNames.Add(P3Name);
	PlayerNames.Add(P4Name);
	PlayerNames.Add(P5Name);
	PlayerNames.Add(P6Name);

	/* Add Crowns to array*/
	Crowns.Add(P1Crown);
	Crowns.Add(P2Crown);
	Crowns.Add(P3Crown);
	Crowns.Add(P4Crown);
	Crowns.Add(P5Crown);
	Crowns.Add(P6Crown);

	/* Set crowns to hidden on game start */
	for (int i = 0; i < Crowns.Num(); i++)
	{
		Crowns[i]->SetVisibility(ESlateVisibility::Hidden);
	}
	
	/* Set starting pickup item */
	ClearPickupUI();

	/* Set interaction text to be hidden on start */
	InteractionUI->SetVisibility(ESlateVisibility::Hidden);
	InteractionText->SetVisibility(ESlateVisibility::Visible);
	interactionButtonTimer = interactionButtonMaxTime;

	/* Non team UI */
	if (GameStateReference->bTeams == false)
	{
		/* Set number of UI shown on screen */
		if (GameStateReference->GetFinalConnectionCount() <= 4)
		{
			// Probably dont need to set visibility at start, but just in case
			OverlayPlayer1->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer2->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer3->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer4->SetVisibility(ESlateVisibility::Visible);
				
			if (GameStateReference->GetFinalConnectionCount() <= 3)
			{
				OverlayPlayer4->SetVisibility(ESlateVisibility::Hidden);

				if (GameStateReference->GetFinalConnectionCount() <= 2)
				{
					OverlayPlayer3->SetVisibility(ESlateVisibility::Hidden);
						
					if (GameStateReference->GetFinalConnectionCount() == 2)
					{
						OverlayPlayer2->SetVisibility(ESlateVisibility::Visible);

					}
					else if (GameStateReference->GetFinalConnectionCount() == 1)
					{
						OverlayPlayer2->SetVisibility(ESlateVisibility::Hidden);
					}
					OverlayPlayer1->SetVisibility(ESlateVisibility::Visible);
				
					UOverlaySlot* overlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]);
					overlaySlot->SetPadding(FMargin(0,0,0,0));
				}
			}
		}

		// Set positions of slots for 1 - 4 players
		if (GameStateReference->GetFinalConnectionCount() == 4 || GameStateReference->GetFinalConnectionCount() == 3)
		{
			UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]); // Change position of player 1
			OverlaySlot->SetPadding(FMargin(0,0,650,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[1]); // Change position of player 2
			OverlaySlot->SetPadding(FMargin(0,0,300,0));
		}
		else
		{
			UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]); // Change position of player 1
			OverlaySlot->SetPadding(FMargin(0,0,400,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[1]); // Change position of player 2
			OverlaySlot->SetPadding(FMargin(400,0,0,0));
		}
		
		/* Change UI positions if more than 4 players*/
		if (GameStateReference->GetFinalConnectionCount() > 4)
		{
			/* Turn all UI icons on (make visible */
			OverlayPlayer1->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer2->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer3->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer4->SetVisibility(ESlateVisibility::Visible);
			OverlayPlayer5->SetVisibility(ESlateVisibility::Visible);
			
			if (GameStateReference->GetFinalConnectionCount() > 4)
				OverlayPlayer6->SetVisibility(ESlateVisibility::Visible);

			/* Set positions of overlay UI (images and scores) for 4+ players
			 * GetSlots()[2] not showing as that is the game timer
			 */
			UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]); // Player 1 UI
			OverlaySlot->SetPadding(FMargin(0,0,780,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[1]); // Player 2 UI
			OverlaySlot->SetPadding(FMargin(0,0,510,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[3]); // Player 3 UI
			OverlaySlot->SetPadding(FMargin(0,0,230,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[4]); // Player 4 UI
			OverlaySlot->SetPadding(FMargin(230,0,0,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[5]); // Player 5 UI
			OverlaySlot->SetPadding(FMargin(510,0,0,0));
			OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[6]); // Player 6 UI
			OverlaySlot->SetPadding(FMargin(780,0,0,0));
		}
	}
	else /* Teams UI */
	{
		/* Turn all UI icons on (make visible */
		OverlayPlayer1->SetVisibility(ESlateVisibility::Hidden);
		OverlayPlayer2->SetVisibility(ESlateVisibility::Hidden);
		OverlayPlayer3->SetVisibility(ESlateVisibility::Hidden);
		OverlayPlayer4->SetVisibility(ESlateVisibility::Hidden);
		OverlayPlayer5->SetVisibility(ESlateVisibility::Hidden);
		OverlayPlayer5->SetVisibility(ESlateVisibility::Hidden);
		
		TeamsOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWidget_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (GameStateReference)
	{
		/* Set UI colours for teams */
		if (ColourData && GameStateReference->bTeams)
		{
			if (ColourData->PureColours.IsValidIndex((int32)GameStateReference->TeamOneColour))
			{
				T1Ring->SetColorAndOpacity(ColourData->PureColours[(int32)GameStateReference->TeamOneColour]); // Red Team
			}
			if (ColourData->PureColours.IsValidIndex((int32)GameStateReference->TeamTwoColour))
			{
				T2Ring->SetColorAndOpacity(ColourData->PureColours[(int32)GameStateReference->TeamTwoColour]); // Blue Team
			}
		}

		/* Game timers related */
		Minutes->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMatchLengthMinutes())));
		//Seconds->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthSeconds)));
		
		int32 MatchSeconds = (int)GameStateReference->GetMatchLengthSeconds();

		if (MatchSeconds < 10)
			Seconds->SetText(FText::FromString("0" + FString::FromInt(MatchSeconds)));
		else
			Seconds->SetText(FText::FromString(FString::FromInt(MatchSeconds)));

		if (GameStateReference->HasGameFinished())
		{
			StartAndEndMenu->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else if (GameStateReference->HasGameStarted())
		{
			StartAndEndMenu->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			StartAndEndMenu->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		
		/* Non team UI */
		if (GameStateReference->bTeams == false)
		{
			// Updating points/coins
			for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
			{
				if (!GameStateReference->Server_Players.IsValidIndex(i))
					continue;
				
				if (auto Player = GameStateReference->Server_Players[i])
				{
					//if (!Player)
					//	return;
					
					auto Coins = Player->Coins;
					auto ExtraCoins = FString::FromInt(Player->ExtraCoins);
					auto bIsShowingExtraCoins = Player->bIsShowingExtraCoins;
				
					//UE_LOG(LogTemp, Warning, TEXT("Player [%s] ID = %s"), *FString::FromInt(i), *FString::FromInt(player->Player_ID));
				
					switch(i)
					{
					case 0:
						{
							Player1Coins->SetText(FText::FromString(FString::FromInt(Coins))); // Set player score
							// Showing coin increase
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = plus + ExtraCoins;
								Player1ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
								Player1ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Player1ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
							}
						
							break;
						}
					case 1:
						{
							Player2Coins->SetText(FText::FromString(FString::FromInt(Coins))); // Set player score
							// Showing coin increase
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = plus + ExtraCoins;
								Player2ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
								Player2ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Player2ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
							}

							break;
						}
					case 2:
						{
							Player3Coins->SetText(FText::FromString(FString::FromInt(Coins)));  // Set player score
							// Showing coin increase
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = plus + ExtraCoins;
								Player3ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
								Player3ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Player3ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
							}

							break;
						}
					case 3:
						{
							Player4Coins->SetText(FText::FromString(FString::FromInt(Coins))); // Set player score
							// Showing coin increase
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = plus + ExtraCoins;
								Player4ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
								Player4ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Player4ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
							}

							break;
						}
					case 4:
						{
							Player5Coins->SetText(FText::FromString(FString::FromInt(Coins))); // Set player score
							// Showing coin increase
							if (bIsShowingExtraCoins == true)
							{
								FString Plus = "+";
								FString Combined = Plus + ExtraCoins;
								Player5ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
								Player5ExtraCoins->SetText(FText::FromString(Combined)); // Set player extra score
							}
							else
							{
								Player5ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
							}

							break;
						}
					case 5:
						{
							Player6Coins->SetText(FText::FromString(FString::FromInt(Coins))); // Set player score
							// Showing coin increase
							if (bIsShowingExtraCoins == true)
							{
								FString Plus = "+";
								FString Combined = Plus + ExtraCoins;
								Player6ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
								Player6ExtraCoins->SetText(FText::FromString(Combined)); // Set player extra score
							}
							else
							{
								Player6ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
							}
							break;
							;
						}
					default:
						{
							break;
						}
					}
				}
			}
		}
		else
		{
			/* Team 1 points */
			if (GameStateReference->Server_TeamOne.Num() == 0)
				return;

			Team1Coins->SetText(FText::FromString(FString::FromInt(GameStateReference->Team1Points))); // Updating team 1 points
			Team2Coins->SetText(FText::FromString(FString::FromInt(GameStateReference->Team2Points))); // Updating team 2 points
			
			for (int i = 0; i < GameStateReference->Server_TeamOne.Num(); i++)
			{
				if (auto Player = GameStateReference->Server_TeamOne[i])
				{
					auto ExtraCoins = FString::FromInt(Player->ExtraCoins);
					auto bIsShowingExtraCoins = Player->bIsShowingExtraCoins;
				
					switch(i)
					{
					case 0:
						{
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = "plus" + ExtraCoins;
								Team1ExtraCoinsP1->SetVisibility(ESlateVisibility::Visible);
							
								Team1ExtraCoinsP1->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Team1ExtraCoinsP1->SetVisibility(ESlateVisibility::Hidden);
							}
						
							break;
						}
					case 1:
						{
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = plus + ExtraCoins;
								Team1ExtraCoinsP2->SetVisibility(ESlateVisibility::Visible);
							
								Team1ExtraCoinsP2->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Team1ExtraCoinsP2->SetVisibility(ESlateVisibility::Hidden);
							}

							break;
						}
					case 2:
						{
							if (bIsShowingExtraCoins == true)
							{
								FString plus = "+";
								FString combined = plus + ExtraCoins;
								Team1ExtraCoinsP3->SetVisibility(ESlateVisibility::Visible);
							
								Team1ExtraCoinsP3->SetText(FText::FromString(combined)); // Set player extra score
							}
							else
							{
								Team1ExtraCoinsP3->SetVisibility(ESlateVisibility::Hidden);
							}

							break;
						}
					default:
						{
							break;
						}
					}
					
				}
			}
		}
		
		if (GameStateReference->IsGameReadyForVote())
		{
			EnableEndgameMenu();
			SetHUDInteractText("");
		}

		SetHUDInteractText("");
		
		if (auto Owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
		{
			if (auto closestInteractable = Owner->ClosestInteractableItem)
			{
				if ( Owner->PlayerStateRef)
				{
					closestInteractable->OnDisplayInteractText(this, Owner, Owner->PlayerStateRef->Player_ID);
				}
			}
		}
	}
	else
	{
		if (auto GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			GameStateReference = GameState;
		}
	}

	InteractionImagePulse(InDeltaTime);

	UpdateEmphasizers(InDeltaTime);
	
	UpdatePlayerIcons();
}

void UWidget_PlayerHUD::EnableDisableMenu()
{
	if (OptionsMenu->GetVisibility() == ESlateVisibility::Hidden)
		IngameMenu->ToggleMenu();
}

void UWidget_PlayerHUD::ShowEmoteRadialMenu()
{
	EmoteRadialMenu->SetVisibility(ESlateVisibility::Visible);
}

void UWidget_PlayerHUD::DisableEmoteRadialMenu()
{
	EmoteRadialMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_PlayerHUD::EnableEndgameMenu()
{
	IngameMenu->DisableMenu();
	EndgameMenu->EnableEndgameMenu();
	bEndgame = true;
}

void UWidget_PlayerHUD::UpdatePickupUI(UTexture2D* _PickupTexture)
{
	if (!_PickupTexture)
	{
		return;
	}
	OverlayPickup->SetVisibility(ESlateVisibility::Visible);
	FVector2D ImageSize ={(float)_PickupTexture->GetSizeX(), (float)_PickupTexture->GetSizeY()};
	PickupImage->SetDesiredSizeOverride(ImageSize);
	PickupImage->SetBrushFromTexture(_PickupTexture);
}

void UWidget_PlayerHUD::ClearPickupUI()
{
	OverlayPickup->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_PlayerHUD::SetHUDInteractText(FString _InteractionText)
{
	if (_InteractionText == "")
	{
		InteractionUI->SetVisibility(ESlateVisibility::Hidden);
		bInteractionButtonShowing = false;
	}
	else
	{
		InteractionUI->SetVisibility(ESlateVisibility::Visible);
		InteractionText->SetText(FText::FromString(_InteractionText));
		bInteractionButtonShowing = true;
	}
}

void UWidget_PlayerHUD::InteractionImagePulse(float _DeltaTime)
{
	if (bInteractionButtonShowing == true)
	{
		interactionButtonTimer -= _DeltaTime;

		if (interactionButtonTimer <= 0)
		{
			if (bShowETexture1 == true)
			{
				bShowETexture1 = false;
				InteractionButtonImage->SetBrushFromTexture(ETexture1);
			}
			else
			{
				bShowETexture1 = true;
				InteractionButtonImage->SetBrushFromTexture(ETexture2);
			}

			interactionButtonTimer = interactionButtonMaxTime;
		}
	}
}

void UWidget_PlayerHUD::SetPlayerSprintTimer(float _SprintTime)
{
	if (_SprintTime < 0.0f)
	{
		//SprintImage->SetBrushFromTexture(CanSprintIconTexture);
	}
	else
	{
		//SprintImage->SetBrushFromTexture(CanNotSprintIconTexture);
	}
}

void UWidget_PlayerHUD::SetWeaponDurability(int32 _Durability)
{
}

void UWidget_PlayerHUD::UpdatePlayerIcons()
{
	for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_Players[i])
		{
			/* Set Ring Colours */
			if (Rings.Num() > i)
			{
				Rings[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));
			}

			/* Set Player Names */
			if (PlayerNames.Num() > i)
			{
				PlayerNames[i]->SetText(FText::FromString(Player->PlayerName));
				PlayerNames[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));

				if (bHasChangedBlackTextOutlineColour == false)
				{
					if (Player->Details.Colour == EColours::BLACK)
					{
						PlayerIDToChangeOutline = Player->Player_ID;
						bHasChangedBlackTextOutlineColour = true;
					}
				}
			}
			
			/* Set Crown visibility */
			if (Crowns.Num() > i)
			{
				if (GameStateReference->GetPlayerWinner() == i)
					Crowns[i]->SetVisibility(ESlateVisibility::Visible);
				else
					Crowns[i]->SetVisibility(ESlateVisibility::Hidden);
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
						UE_LOG(LogTemp, Warning, TEXT("Error: Widget_PlayerHUD: Unable to determine character type"));
						break;
					}
				}
			}
		}
	}
}


void UWidget_PlayerHUD::UpdateEmphasizers(float _DeltaTime)
{
	if (Emphasizers.IsEmpty())
		return;

	for (FEmphasizer Emphasizer : Emphasizers)
	{
		if (Emphasizer.bHasEmphasized)
		{
			// Shrink
			FVector2D ShrunkVector;
			ShrunkVector.X = Emphasizer.Image->GetRenderTransform().Scale.X - _DeltaTime * Emphasizer.Speed;
			ShrunkVector.Y = Emphasizer.Image->GetRenderTransform().Scale.Y - _DeltaTime * Emphasizer.Speed;
			Emphasizer.Image->SetRenderScale(ShrunkVector);
			
			// If Image has reached original scale, remove
			if (Emphasizer.Image->GetRenderTransform().Scale.Length() <= Emphasizer.OriginalScale.Length())
			{
				Emphasizer.Image->SetRenderScale(Emphasizer.OriginalScale);
				Emphasizers.Remove(Emphasizer);
				continue;
			}
		}

		// Grow
		FVector2d GrownVector;
		GrownVector.X = Emphasizer.Image->GetRenderTransform().Scale.X + _DeltaTime * Emphasizer.Speed;
		GrownVector.Y = Emphasizer.Image->GetRenderTransform().Scale.Y + _DeltaTime * Emphasizer.Speed;
		Emphasizer.Image->SetRenderScale(GrownVector);

		if (GrownVector.Length() > Emphasizer.DesiredScale.Length())
		{
			Emphasizer.bHasEmphasized = true;
		}
	}
}

