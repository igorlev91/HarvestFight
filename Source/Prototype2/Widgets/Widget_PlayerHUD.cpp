// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_PlayerHUD.h"

#include "Widget_EndgameMenu.h"
#include "Widget_IngameMenu.h"
#include "Components/Image.h"
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
		Seconds->SetText(FText::FromString(FString::FromInt(GameStateRef->MatchLengthSeconds)));

		// Updating points/coins
		//if (!GetOwningPlayerPawn()->HasAuthority())
		//	UE_LOG(LogTemp, Warning, TEXT("Players Array Size = %s"), *FString::FromInt(GameStateRef->PlayerArray.Num()));

		for (int i = 0; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto player = GameStateRef->Server_Players[i])
			{
				auto coins = player->Coins;
				UE_LOG(LogTemp, Warning, TEXT("Player [%s] ID = %s"), *FString::FromInt(i), *FString::FromInt(player->Player_ID));
								
				switch(i)
				{
				case 0:
					{
						Player1Coins->SetText(FText::FromString(FString::FromInt(coins)));
											
						break;
					}
				case 1:
					{
						Player2Coins->SetText(FText::FromString(FString::FromInt(coins)));
											
						break;
					}
				case 2:
					{
						Player3Coins->SetText(FText::FromString(FString::FromInt(coins)));
											
						break;
					}
				case 3:
					{
						Player4Coins->SetText(FText::FromString(FString::FromInt(coins)));
											
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
		
		if (auto* playerController = Cast<APrototype2PlayerController>(GetOwningPlayerPawn()->GetController()))
		{
			auto playerID = playerController->GetPlayerState<APrototype2PlayerState>()->Player_ID;
			if (GameStateRef->Server_Players.Num() >= playerID)
			{
				if (auto* playerState = Cast<APrototype2PlayerState>(GameStateRef->Server_Players[playerID]))
				{
					SetHUDInteractText("");
					if (auto* owner = GetOwningPlayerPawn<APrototype2Character>())
					{
						if (auto* closestInteractable = owner->ClosestInteractableItem)
						{
							switch (closestInteractable->InterfaceType)
							{
							case EInterfaceType::SellBin:
								{
									// Set to "Sell"
									if(auto heldItem = owner->HeldItem)
									{
										if (heldItem->ItemComponent->PickupType == EPickup::Cabbage ||
											heldItem->ItemComponent->PickupType == EPickup::Carrot ||
											heldItem->ItemComponent->PickupType == EPickup::Mandrake)
										{
											SetHUDInteractText("Sell");
											break;
										}
									}
									break;
								}
							case EInterfaceType::GrowSpot:
								{
									if (auto* growSpot = Cast<AGrowSpot>(closestInteractable))
									{
										if (growSpot->Player_ID == playerState->Player_ID)
										{
											switch (closestInteractable->GrowSpotState)
											{
											case EGrowSpotState::Empty:
												{
													// Set to "Grow"
													if(auto heldItem = owner->HeldItem)
													{
														if (heldItem->ItemComponent->PickupType == EPickup::CabbageSeed ||
															heldItem->ItemComponent->PickupType == EPickup::CarrotSeed ||
															 heldItem->ItemComponent->PickupType == EPickup::MandrakeSeed)
														{
															SetHUDInteractText("Grow");
															break;
														}
													}
													break;
												}
											case EGrowSpotState::Growing:
												{
													break;
												}
											case EGrowSpotState::Grown:
												{
													if (!owner->HeldItem)
													{
														// Set to "Grow"
														SetHUDInteractText("Pick Up");
													}
													break;
												}
											case EGrowSpotState::Default:
												{
													// Pass through
												}
											default:
												{
													// Set to none
													break;
												}
											}						
										}
									}
									break;
								}
							case EInterfaceType::Default:
								{
									// Set to "Sell"
									if (!owner->HeldItem)
									{
										SetHUDInteractText("Pick Up");
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



