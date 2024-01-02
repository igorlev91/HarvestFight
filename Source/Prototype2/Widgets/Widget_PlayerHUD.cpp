// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_PlayerHUD.h"

#include "Widget_EndgameMenu.h"
#include "Widget_IngameMenu.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

void UWidget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gameState;
	}
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
				if (auto* playerState = Cast<APrototype2PlayerState>(player))
				{
					auto coins = playerState->Coins;
					if (!GetOwningPlayerPawn()->HasAuthority())
					{
						UE_LOG(LogTemp, Warning, TEXT("Players Array Size = %s"), *FString::FromInt(GameStateRef->Server_Players.Num()));
						UE_LOG(LogTemp, Warning, TEXT("Players Array [%s] = %s"), *FString::FromInt(i), *FString::FromInt(coins));
					}
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
		}
		if (GameStateRef->MatchLengthMinutes <= 0 && GameStateRef->MatchLengthSeconds <= 0)
		{
			EnableEndgameMenu();
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
	case Cabbage:
		{
			PickupImage->SetBrushFromTexture(CabbageTexture);
			break;
		}
	case Mandrake:
		{
			PickupImage->SetBrushFromTexture(MandrakeTexture);
			break;
		}
	default:
		{
			
		}
	}
	
}



