// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_PlayerHUD.h"
#include "Widget_IngameMenu.h"
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
	}

	//for(auto player:GameStateRef->PlayerArray)
	//{
	//	player->GetPlayerController()->GetPlayerState<APrototype2PlayerState>()->Coins;
	//}

	for (int i = 0; i < GameStateRef->PlayerArray.Num(); i++)
	{
		switch(i)
		{
		case 0:
			{
				//auto coins = GameStateRef->PlayerArray[i]->GetPlayerController()->GetPlayerState<APrototype2PlayerState>()->Coins;
				//Coins->SetText(FText::FromString(FString::FromInt(coins)));
				break;
			}

		default:
			{
				break;
			}
		}
	}
}

void UWidget_PlayerHUD::EnableDisableMenu()
{
	IngameMenu->EnableDisableMenu();
}
