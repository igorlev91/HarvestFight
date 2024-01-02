// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_PlayerHUD.h"

#include "Widget_EndgameMenu.h"
#include "Widget_IngameMenu.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/GrowSpot.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Prototype2Character.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PickUpItem.h"
#include "Prototype2/Prototype2PlayerController.h"
#include "Widgets/SOverlay.h"
#include "Layout/Margin.h"

void UWidget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gameState;
	}

	// Set starting pickup item
	UpdatePickupUI(None);

	// Set interaction text to be hidden on start
	InteractionText->SetVisibility(ESlateVisibility::Hidden);

	
}

void UWidget_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameStateRef)
	{
		Minutes->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthMinutes)));
		//Seconds->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthSeconds)));
		
		int seconds = (int)GameStateRef->MatchLengthSeconds;

		if (seconds == 1)
		{
			Seconds->SetText(FText::FromString("01"));
		}
		else if (seconds == 2)
		{
			Seconds->SetText(FText::FromString("02"));
		}
		else if (seconds == 3)
		{
			Seconds->SetText(FText::FromString("03"));
		}
		else if (seconds == 4)
		{
			Seconds->SetText(FText::FromString("04"));
		}
		else if (seconds == 5)
		{
			Seconds->SetText(FText::FromString("05"));
		}
		else if (seconds == 6)
		{
			Seconds->SetText(FText::FromString("06"));
		}
		else if (seconds == 7)
		{
			Seconds->SetText(FText::FromString("07"));
		}
		else if (seconds == 8)
		{
			Seconds->SetText(FText::FromString("08"));
		}
		else if (seconds == 9)
		{
			Seconds->SetText(FText::FromString("09"));
		}
		else if (seconds == 0)
		{
			Seconds->SetText(FText::FromString("00"));
		}
		else
		{
			Seconds->SetText(FText::FromString(FString::FromInt(seconds)));
			//Seconds->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthSeconds)));
		}

		// Set number of UI shown on screen
		if (GameStateRef->Server_Players.Num() <= 4)
		{
			Overlay_P1->SetVisibility(ESlateVisibility::Visible);
			Overlay_P2->SetVisibility(ESlateVisibility::Visible);
			Overlay_P3->SetVisibility(ESlateVisibility::Visible);
			Overlay_P4->SetVisibility(ESlateVisibility::Visible);
			
			if (GameStateRef->Server_Players.Num() <= 3)
			{
				Overlay_P4->SetVisibility(ESlateVisibility::Hidden);

				if (GameStateRef->Server_Players.Num() <= 2)
				{
					Overlay_P3->SetVisibility(ESlateVisibility::Hidden);
					
					if (GameStateRef->Server_Players.Num() == 2)
					{
						Overlay_P2->SetVisibility(ESlateVisibility::Visible);

					}
					else if (GameStateRef->Server_Players.Num() == 1)
					{
						Overlay_P2->SetVisibility(ESlateVisibility::Hidden);
					}
					Overlay_P1->SetVisibility(ESlateVisibility::Visible);
			
					UOverlaySlot* overlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]);
					overlaySlot->SetPadding(FMargin(0,0,0,0));
				}
			}
		}

		// Set positions of slots
		if (GameStateRef->Server_Players.Num() == 4 || GameStateRef->Server_Players.Num() == 3)
		{
			UOverlaySlot* overlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]); // Change position of player 1
			overlaySlot->SetPadding(FMargin(0,0,650,0));
			overlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[1]); // Change position of player 2
			overlaySlot->SetPadding(FMargin(0,0,300,0));
		}
		else
		{
			UOverlaySlot* overlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[0]); // Change position of player 1
			overlaySlot->SetPadding(FMargin(0,0,400,0));
			overlaySlot = CastChecked<UOverlaySlot>(TopOverlayUI->GetSlots()[1]); // Change position of player 2
			overlaySlot->SetPadding(FMargin(400,0,0,0));
		}
		
		// Updating points/coins
		//if (!GetOwningPlayerPawn()->HasAuthority())
		//	UE_LOG(LogTemp, Warning, TEXT("Players Array Size = %s"), *FString::FromInt(GameStateRef->PlayerArray.Num()));

		for (int i = 0; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto player = GameStateRef->Server_Players[i])
			{
				auto coins = player->Coins;
				auto extraCoins = FString::FromInt(player->ExtraCoins);
				auto isShowingExtraCoins = player->IsShowingExtraCoins;
				
				//UE_LOG(LogTemp, Warning, TEXT("Player [%s] ID = %s"), *FString::FromInt(i), *FString::FromInt(player->Player_ID));
				
				//P2Icon->SetBrushFromTexture(PlayerIcons[4]);
				//P3Icon->SetBrushFromTexture(PlayerIcons[4]);
				//P4Icon->SetBrushFromTexture(PlayerIcons[4]);
				switch(i)
				{
				case 0:
					{
						Player1Coins->SetText(FText::FromString(FString::FromInt(coins))); // Set player score
						// Showing coin increase
						if (isShowingExtraCoins == true)
						{
							FString plus = "+";
							FString combined = plus + extraCoins;
							Player1ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
							Player1ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
						}
						else
						{
							Player1ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
						}
						
						//if (GameStateRef->Server_Players.Num() >= 1)
						//	P1Icon->SetBrushFromTexture(PlayerIcons[0]);
						break;
					}
				case 1:
					{
						Player2Coins->SetText(FText::FromString(FString::FromInt(coins)));
						// Showing coin increase
						if (isShowingExtraCoins == true)
						{
							FString plus = "+";
							FString combined = plus + extraCoins;
							Player2ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
							Player2ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
						}
						else
						{
							Player2ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
						}
						
						//if (GameStateRef->Server_Players.Num() >= 2)
						//	P2Icon->SetBrushFromTexture(PlayerIcons[1]);
						break;
					}
				case 2:
					{
						Player3Coins->SetText(FText::FromString(FString::FromInt(coins)));
						// Showing coin increase
						if (isShowingExtraCoins == true)
						{
							FString plus = "+";
							FString combined = plus + extraCoins;
							Player3ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
							Player3ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
						}
						else
						{
							Player3ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
						}
						//if (GameStateRef->Server_Players.Num() >= 3)
						//	P3Icon->SetBrushFromTexture(PlayerIcons[2]);
						break;
					}
				case 3:
					{
						Player4Coins->SetText(FText::FromString(FString::FromInt(coins)));
						// Showing coin increase
						if (isShowingExtraCoins == true)
						{
							FString plus = "+";
							FString combined = plus + extraCoins;
							Player4ExtraCoins->SetVisibility(ESlateVisibility::Visible);
							
							Player4ExtraCoins->SetText(FText::FromString(combined)); // Set player extra score
						}
						else
						{
							Player4ExtraCoins->SetVisibility(ESlateVisibility::Hidden);
						}
						//if (GameStateRef->Server_Players.Num() >= 4)
						//	P4Icon->SetBrushFromTexture(PlayerIcons[3]);
						break;
					}
				default:
					{
						break;
					}
				}
			}
		}
		if (GameStateRef->MatchLengthMinutes <= 0 && GameStateRef->MatchLengthSeconds <= 0)
		{
			EnableEndgameMenu();
		}

		if (auto* owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
		{
			SetHUDInteractText("");
			if (auto* closestInteractable = owner->ClosestInteractableItem)
			{
				closestInteractable->OnDisplayInteractText(this, owner, owner->PlayerID);
			}
		}
	}
}

void UWidget_PlayerHUD::EnableDisableMenu()
{
	IngameMenu->EnableDisableMenu();
}

void UWidget_PlayerHUD::EnableEndgameMenu()
{
	IngameMenu->DisableMenu();
	EndgameMenu->EnableEndgameMenu();
}

void UWidget_PlayerHUD::UpdatePickupUI(EPickup _pickup)
{
	if (_pickup != None)
	{
		if (PickupImage->GetVisibility() == ESlateVisibility::Hidden)
		{
			PickupImage->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	switch(_pickup)
	{
	case None:
		{
			PickupImage->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	case Carrot:
		{
			PickupImage->SetBrushFromTexture(CarrotTexture);
			break;
		}
	case CarrotSeed:
		{
			PickupImage->SetBrushFromTexture(CarrotSeedTexture);
			break;
		}
	case Cabbage:
		{
			PickupImage->SetBrushFromTexture(CabbageTexture);
			break;
		}
	case CabbageSeed:
		{
			PickupImage->SetBrushFromTexture(CabbageSeedTexture);
			break;
		}
	case Mandrake:
		{
			PickupImage->SetBrushFromTexture(MandrakeTexture);
			break;
		}
	case MandrakeSeed:
		{
			PickupImage->SetBrushFromTexture(MandrakeSeedTexture);
			break;
		}
	case Weapon:
		{
			WeaponImage->SetVisibility(ESlateVisibility::Visible);
			break;
		}
	case NoWeapon:
		{
			WeaponImage->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	default:
		{
			
		}
	}
	
}

void UWidget_PlayerHUD::UpdateWeaponUI(EPickup _weapon)
{
	switch(_weapon)
	{
	case Weapon:
		{
			WeaponImage->SetVisibility(ESlateVisibility::Visible);
			break;
		}
	case NoWeapon:
		{
			WeaponImage->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	default:
		{
			
		}
	}
}

void UWidget_PlayerHUD::SetHUDInteractText(FString _interactionText)
{
	if (_interactionText == "")
	{
		InteractionText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		InteractionText->SetVisibility(ESlateVisibility::Visible);
		InteractionText->SetText(FText::FromString(_interactionText));
	}
}

void UWidget_PlayerHUD::SetPlayerSprintTimer(float _sprintTime)
{
	if (_sprintTime < 0.0f)
	{
		//SprintImage->SetBrushFromTexture(CanSprintIconTexture);
	}
	else
	{
		//SprintImage->SetBrushFromTexture(CanNotSprintIconTexture);
	}
}

void UWidget_PlayerHUD::SetWeaponDurability(int _durability)
{
}



