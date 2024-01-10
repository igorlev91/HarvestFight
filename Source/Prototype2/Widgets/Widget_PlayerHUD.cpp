

#include "Widget_PlayerHUD.h"

#include "Widget_EndgameMenu.h"
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
#include "Prototype2/GameInstances/PrototypeGameInstance.h"

void UWidget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/* Get gamestate and set to reference variable */
	if (auto* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}

	/* Set all rings to white */
	if (RingTexture)
	{
		P1Ring->SetBrushFromTexture(RingTexture);
		P2Ring->SetBrushFromTexture(RingTexture);
		P3Ring->SetBrushFromTexture(RingTexture);
		P4Ring->SetBrushFromTexture(RingTexture);
	}
	/* Add rings to array */
	Rings.Add(P1Ring);
	Rings.Add(P2Ring);
	Rings.Add(P3Ring);
	Rings.Add(P4Ring);

	/* Set ring colours to player colour */
	for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_Players[i])
		{
			if (Rings.Num() > i)
			{
				Rings[i]->SetColorAndOpacity((FLinearColor)(Player->Details.CharacterColour));
			}
		}
	}
	
	/* Set starting pickup item */
	ClearPickupUI();

	/* Set interaction text to be hidden on start */
	InteractionUI->SetVisibility(ESlateVisibility::Hidden);
	InteractionText->SetVisibility(ESlateVisibility::Visible);
	interactionButtonTimer = interactionButtonMaxTime;
	
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
		OverlayPlayer6->SetVisibility(ESlateVisibility::Visible);

		/* Set positions of overlay UI (images and scores) for 4+ players
		 * GetSlots()[2] not showing as that is the game timer
		 */
		UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]); // Player 1 UI
		OverlaySlot->SetPadding(FMargin(0,0,780,0));
		OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[1]); // Player 2 UI
		OverlaySlot->SetPadding(FMargin(0,0,500,0));
		OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[3]); // Player 3 UI
		OverlaySlot->SetPadding(FMargin(0,0,220,0));
		OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[4]); // Player 4 UI
		OverlaySlot->SetPadding(FMargin(220,0,0,0));
		OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[5]); // Player 5 UI
		OverlaySlot->SetPadding(FMargin(500,0,0,0));
		OverlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[6]); // Player 6 UI
		OverlaySlot->SetPadding(FMargin(780,0,0,0));
	}
}

void UWidget_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (GameStateReference)
	{
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
		
		// Updating points/coins
		//if (!GetOwningPlayerPawn()->HasAuthority())
		//	UE_LOG(LogTemp, Warning, TEXT("Players Array Size = %s"), *FString::FromInt(GameStateRef->PlayerArray.Num()));

		for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
		{
			if (auto Player = GameStateReference->Server_Players[i])
			{
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
						
						//if (GameStateRef->Server_Players.Num() >= 1)
						//	P1Icon->SetBrushFromTexture(PlayerIcons[0]);
						SetPlayerIcons(1, Player);
						
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
						
						//if (GameStateRef->Server_Players.Num() >= 2)
						//	P2Icon->SetBrushFromTexture(PlayerIcons[1]);
						SetPlayerIcons(2, Player);
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
						//if (GameStateRef->Server_Players.Num() >= 3)
						//	P3Icon->SetBrushFromTexture(PlayerIcons[2]);
						SetPlayerIcons(3, Player);
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
						//if (GameStateRef->Server_Players.Num() >= 4)
						//	P4Icon->SetBrushFromTexture(PlayerIcons[3]);
						SetPlayerIcons(4, Player);
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
						SetPlayerIcons(5, Player);
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
						SetPlayerIcons(6, Player);
						break;
					}
				default:
					{
						break;
					}
				}
			}
		}
		if (GameStateReference->IsGameReadyForVote())
		{
			EnableEndgameMenu();
			SetHUDInteractText("");
		}
		
		if (auto Owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
		{
			if (auto closestInteractable = Owner->ClosestInteractableItem)
			{
				if ( Owner->PlayerStateRef)
				{
					closestInteractable->OnDisplayInteractText(this, Owner, Owner->PlayerStateRef->Player_ID);
				}
			}
			else
			{
				SetHUDInteractText("");
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
}

void UWidget_PlayerHUD::EnableDisableMenu()
{
	IngameMenu->ToggleMenu();
}

void UWidget_PlayerHUD::EnableEndgameMenu()
{
	IngameMenu->DisableMenu();
	EndgameMenu->EnableEndgameMenu();
	bEndgame = true;
}

void UWidget_PlayerHUD::UpdatePickupUI(UTexture2D* _PickupTexture)
{
	OverlayPickup->SetVisibility(ESlateVisibility::Visible);
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

void UWidget_PlayerHUD::SetPlayerIcons(int32 _IconNumber, APrototype2PlayerState* _Player)
{
	//int PlayerNumber = _IconNumber - 1;
	//if (PlayerNumber <= 0)
	//	PlayerNumber = 0;
	//
	//switch (PlayerNumber)
	//{
	//case 0: // Player 1 Icon
	//	P1Icon->SetBrushFromTexture(SetIcon(_Player));
	//	break;
	//case 1:
	//	P2Icon->SetBrushFromTexture(SetIcon(_Player));
	//	break;
	//case 2:
	//	P3Icon->SetBrushFromTexture(SetIcon(_Player));
	//	break;
	//case 3:
	//	P4Icon->SetBrushFromTexture(SetIcon(_Player));
	//	break;
	//default:
	//	break;
	//}
}

UTexture2D* UWidget_PlayerHUD::SetIcon(APrototype2PlayerState* _Player)
{
	//switch (_Player->Character)
	//{
	//case ECharacters::COW:
	//	{
	//		switch (_Player->CharacterColour)
	//		{
	//		case ECharacterColours::RED:
	//			return Cow_Red_Texture;
	//			break;
	//		case ECharacterColours::BLUE:
	//			return Cow_Blue_Texture;
	//			break;
	//		case ECharacterColours::GREEN:
	//			return Cow_Green_Texture;
	//			break;
	//		case ECharacterColours::YELLOW:
	//			return Cow_Yellow_Texture;
	//			break;
	//		default:
	//			return Cow_Red_Texture;
	//			break;
	//		}
	//		break;
	//	}
	//case ECharacters::PIG:
	//	{
	//		switch (_Player->CharacterColour)
	//		{
	//		case ECharacterColours::RED:
	//			return Pig_Red_Texture;
	//			break;
	//		case ECharacterColours::BLUE:
	//			return Pig_Blue_Texture;
	//			break;
	//		case ECharacterColours::GREEN:
	//			return Pig_Green_Texture;
	//			break;
	//		case ECharacterColours::YELLOW:
	//			return Pig_Yellow_Texture;
	//			break;
	//		default:
	//			return Pig_Red_Texture;
	//			break;
	//		}
	//		break;
	//	}
	//case ECharacters::CHICKEN:
	//	{
	//		switch (_Player->CharacterColour)
	//		{
	//		case ECharacterColours::RED:
	//			return Chicken_Red_Texture;
	//			break;
	//		case ECharacterColours::BLUE:
	//			return Chicken_Blue_Texture;
	//			break;
	//		case ECharacterColours::GREEN:
	//			return Chicken_Green_Texture;
	//			break;
	//		case ECharacterColours::YELLOW:
	//			return Chicken_Yellow_Texture;
	//			break;
	//		default:
	//			return Chicken_Red_Texture;
	//			break;
	//		}
	//		break;
	//	}
	//case ECharacters::DUCK:
	//	{
	//		switch (_Player->CharacterColour)
	//		{
	//		case ECharacterColours::RED:
	//			return Duck_Red_Texture;
	//			break;
	//		case ECharacterColours::BLUE:
	//			return Duck_Blue_Texture;
	//			break;
	//		case ECharacterColours::GREEN:
	//			return Duck_Green_Texture;
	//			break;
	//		case ECharacterColours::YELLOW:
	//			return Duck_Yellow_Texture;
	//			break;
	//		default:
	//			return Duck_Red_Texture;
	//			break;
	//		}
	//		break;
	//	}
	//default:
	//	return Cow_Red_Texture;
	//	break;
	//}

	return nullptr;
}

