// Fill out your copyright notice in the Description page of Project Settings.


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
#include "Prototype2/GrowSpot.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Prototype2Character.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PickUpItem.h"
#include "Prototype2/Prototype2PlayerController.h"
#include "Widgets/SOverlay.h"
#include "Layout/Margin.h"
#include "Prototype2/PrototypeGameInstance.h"

void UWidget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gameState;
	}

	// Set starting pickup item
	UpdatePickupUI(None, false);

	// Set interaction text to be hidden on start
	InteractionUI->SetVisibility(ESlateVisibility::Hidden);
	InteractionText->SetVisibility(ESlateVisibility::Visible);
	interactionButtonTimer = interactionButtonMaxTime;

	
	// Set number of UI shown on screen
	if (GameStateRef->FinalConnectionCount <= 4)
	{
		Overlay_P1->SetVisibility(ESlateVisibility::Visible);
		Overlay_P2->SetVisibility(ESlateVisibility::Visible);
		Overlay_P3->SetVisibility(ESlateVisibility::Visible);
		Overlay_P4->SetVisibility(ESlateVisibility::Visible);
			
		if (GameStateRef->FinalConnectionCount <= 3)
		{
			Overlay_P4->SetVisibility(ESlateVisibility::Hidden);

			if (GameStateRef->FinalConnectionCount <= 2)
			{
				Overlay_P3->SetVisibility(ESlateVisibility::Hidden);
					
				if (GameStateRef->FinalConnectionCount == 2)
				{
					Overlay_P2->SetVisibility(ESlateVisibility::Visible);

				}
				else if (GameStateRef->FinalConnectionCount == 1)
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
	if (GameStateRef->FinalConnectionCount == 4 || GameStateRef->FinalConnectionCount == 3)
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
	
}

void UWidget_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (GameStateRef)
	{
		Minutes->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthMinutes)));
		//Seconds->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthSeconds)));
		
		int seconds = (int)GameStateRef->MatchLengthSeconds;

		if (seconds < 10)
			Seconds->SetText(FText::FromString("0" + FString::FromInt(seconds)));
		else
			Seconds->SetText(FText::FromString(FString::FromInt(seconds)));

		if (GameStateRef->HasGameFinished)
		{
			StartAndEndMenu->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else if (GameStateRef->GameHasStarted)
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

		for (int i = 0; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto player = GameStateRef->Server_Players[i])
			{
				auto coins = player->Coins;
				auto extraCoins = FString::FromInt(player->ExtraCoins);
				auto isShowingExtraCoins = player->IsShowingExtraCoins;
				
				//UE_LOG(LogTemp, Warning, TEXT("Player [%s] ID = %s"), *FString::FromInt(i), *FString::FromInt(player->Player_ID));
				
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
						SetPlayerIcons(1, player);
						
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
						SetPlayerIcons(2, player);
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
						SetPlayerIcons(3, player);
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
						SetPlayerIcons(4, player);
						break;
					}
				default:
					{
						break;
					}
				}
			}
		}
		if (GameStateRef->GameReadyForVote)
		{
			EnableEndgameMenu();
			SetHUDInteractText("");
		}
		
		if (auto owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
		{
			if (auto closestInteractable = owner->ClosestInteractableItem)
			{
				
				closestInteractable->OnDisplayInteractText(this, owner, owner->PlayerID);
			}
			else
			{
				SetHUDInteractText("");
			}
		}
	}
	else
	{
		if (auto gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			GameStateRef = gameState;
		}
	}

	InteractionImagePulse(InDeltaTime);
}

void UWidget_PlayerHUD::EnableDisableMenu()
{
	IngameMenu->EnableDisableMenu();
}

void UWidget_PlayerHUD::EnableEndgameMenu()
{
	IngameMenu->DisableMenu();
	EndgameMenu->EnableEndgameMenu();
	bEndgame = true;
}

void UWidget_PlayerHUD::UpdatePickupUI(EPickup _pickup, bool _isGold)
{
	if (_pickup != None)
	{
		//if (PickupImage->GetVisibility() == ESlateVisibility::Hidden)
		//{
		//	PickupImage->SetVisibility(ESlateVisibility::Visible);
		//}

		if (OverlayPickup->GetVisibility() == ESlateVisibility::Hidden)
		{
			OverlayPickup->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	switch(_pickup)
	{
	case None:
		{
			//PickupImage->SetVisibility(ESlateVisibility::Hidden);
			OverlayPickup->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
	case Carrot:
		{
			if (_isGold)
			{
				PickupImage->SetBrushFromTexture(CarrotGoldTexture);
			}
			else
			{
				PickupImage->SetBrushFromTexture(CarrotTexture);
			}
			break;
		}
	case CarrotSeed:
		{
			PickupImage->SetBrushFromTexture(CarrotSeedTexture);
			break;
		}
	case Cabbage:
		{
			if (_isGold)
			{
				PickupImage->SetBrushFromTexture(CabbageGoldTexture);
			}
			else
			{
				PickupImage->SetBrushFromTexture(CabbageTexture);
			}
			
			break;
		}
	case CabbageSeed:
		{
			PickupImage->SetBrushFromTexture(CabbageSeedTexture);
			break;
		}
	case Mandrake:
		{
			if (_isGold)
			{
				PickupImage->SetBrushFromTexture(MandrakeGoldTexture);
			}
			else
			{
				PickupImage->SetBrushFromTexture(MandrakeTexture);
			}
			
			break;
		}
	case MandrakeSeed:
		{
			PickupImage->SetBrushFromTexture(MandrakeSeedTexture);
			break;
		}
	case Broccoli:
		{
			if (_isGold)
			{
				PickupImage->SetBrushFromTexture(BroccoliGoldTexture);
			}
			else
			{
				PickupImage->SetBrushFromTexture(BroccoliTexture);
			}
			
			break;
		}
	case BroccoliSeed:
		{
			PickupImage->SetBrushFromTexture(BroccoliSeedTexture);
			break;
		}
	case Daikon:
		{
			if (_isGold)
			{
				PickupImage->SetBrushFromTexture(DaikonGoldTexture);
			}
			else
			{
				PickupImage->SetBrushFromTexture(DaikonTexture);
			}
			
			break;
		}
	case DaikonSeed:
		{
			PickupImage->SetBrushFromTexture(DaikonSeedTexture);
			break;
		}
	case Radish:
		{
			if (_isGold)
			{
				PickupImage->SetBrushFromTexture(RadishGoldTexture);
			}
			else
			{
				PickupImage->SetBrushFromTexture(RadishTexture);
			}
			
			break;
		}
	case RadishSeed:
		{
			PickupImage->SetBrushFromTexture(RadishSeedTexture);
			break;
		}
	//case Weapon:
	//	{
	//		WeaponImage->SetVisibility(ESlateVisibility::Visible);
	//		break;
	//	}
	//case NoWeapon:
	//	{
	//		WeaponImage->SetVisibility(ESlateVisibility::Hidden);
	//		break;
	//	}
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
		InteractionUI->SetVisibility(ESlateVisibility::Hidden);
		bInteractionButtonShowing = false;
	}
	else
	{
		InteractionUI->SetVisibility(ESlateVisibility::Visible);
		InteractionText->SetText(FText::FromString(_interactionText));
		bInteractionButtonShowing = true;
	}
}

void UWidget_PlayerHUD::InteractionImagePulse(float _dt)
{
	if (bInteractionButtonShowing == true)
	{
		interactionButtonTimer -= _dt;

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

void UWidget_PlayerHUD::SetPlayerIcons(int _iconNum, APrototype2PlayerState* _player)
{
	int playerNum = _iconNum - 1;
	if (playerNum <= 0)
		playerNum = 0;
	
	switch (playerNum)
	{
	case 0: // Player 1 Icon
		P1Icon->SetBrushFromTexture(SetIcon(_player));
		//UE_LOG(LogTemp, Warning, TEXT("P1"));
		//UE_LOG(LogTemp, Warning, TEXT("Character %d"), _player->Character);
		//UE_LOG(LogTemp, Warning, TEXT("CharacterColour %d"), _player->CharacterColour);
		break;
	case 1:
		P2Icon->SetBrushFromTexture(SetIcon(_player));
		//UE_LOG(LogTemp, Warning, TEXT("P2"));
		//UE_LOG(LogTemp, Warning, TEXT("Character %d"), _player->Character);
		//UE_LOG(LogTemp, Warning, TEXT("CharacterColour %d"), _player->CharacterColour);
		break;
	case 2:
		P3Icon->SetBrushFromTexture(SetIcon(_player));
		//UE_LOG(LogTemp, Warning, TEXT("P3"));
		//UE_LOG(LogTemp, Warning, TEXT("Character %d"), _player->Character);
		//UE_LOG(LogTemp, Warning, TEXT("CharacterColour %d"), _player->CharacterColour);
		break;
	case 3:
		P4Icon->SetBrushFromTexture(SetIcon(_player));
		//UE_LOG(LogTemp, Warning, TEXT("P4"));
		//UE_LOG(LogTemp, Warning, TEXT("Character %d"), _player->Character);
		//UE_LOG(LogTemp, Warning, TEXT("CharacterColour %d"), _player->CharacterColour);
		break;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("NONE"));
		break;
	}
}

UTexture2D* UWidget_PlayerHUD::SetIcon(APrototype2PlayerState* _player)
{
	switch (_player->Character)
	{
	case ECharacters::COW:
		{
			switch (_player->CharacterColour)
			{
			case ECharacterColours::RED:
				//UE_LOG(LogTemp, Warning, TEXT("RED COW"));
				return Cow_Red_Texture;
				break;
			case ECharacterColours::BLUE:
				//UE_LOG(LogTemp, Warning, TEXT("BLUE COW"));
				return Cow_Blue_Texture;
				break;
			case ECharacterColours::GREEN:
				//UE_LOG(LogTemp, Warning, TEXT("GREEN COW"));
				return Cow_Green_Texture;
				break;
			case ECharacterColours::YELLOW:
				//UE_LOG(LogTemp, Warning, TEXT("YELLOW COW"));
				return Cow_Yellow_Texture;
				break;
			default:
				//UE_LOG(LogTemp, Warning, TEXT("DEFAULT COW"));
				return Cow_Red_Texture;
				break;
			}
			break;
		}
	case ECharacters::PIG:
		{
			switch (_player->CharacterColour)
			{
			case ECharacterColours::RED:
				return Pig_Red_Texture;
				break;
			case ECharacterColours::BLUE:
				return Pig_Blue_Texture;
				break;
			case ECharacterColours::GREEN:
				return Pig_Green_Texture;
				break;
			case ECharacterColours::YELLOW:
				return Pig_Yellow_Texture;
				break;
			default:
				return Pig_Red_Texture;
				break;
			}
			break;
		}
	case ECharacters::CHICKEN:
		{
			switch (_player->CharacterColour)
			{
			case ECharacterColours::RED:
				return Chicken_Red_Texture;
				break;
			case ECharacterColours::BLUE:
				return Chicken_Blue_Texture;
				break;
			case ECharacterColours::GREEN:
				return Chicken_Green_Texture;
				break;
			case ECharacterColours::YELLOW:
				return Chicken_Yellow_Texture;
				break;
			default:
				return Chicken_Red_Texture;
				break;
			}
			break;
		}
	case ECharacters::DUCK:
		{
			switch (_player->CharacterColour)
			{
			case ECharacterColours::RED:
				return Duck_Red_Texture;
				break;
			case ECharacterColours::BLUE:
				return Duck_Blue_Texture;
				break;
			case ECharacterColours::GREEN:
				return Duck_Green_Texture;
				break;
			case ECharacterColours::YELLOW:
				return Duck_Yellow_Texture;
				break;
			default:
				return Duck_Red_Texture;
				break;
			}
			break;
		}
	default:
		//UE_LOG(LogTemp, Warning, TEXT("CHARACTER DEFAULT"));
		return Cow_Red_Texture;
		break;
	}
}

