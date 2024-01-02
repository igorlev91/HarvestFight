// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_EndgameMenu.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Prototype2PlayerState.h"

void UWidget_EndgameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gameState;
	}
}

void UWidget_EndgameMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateWinnerText();

}

void UWidget_EndgameMenu::UpdateWinnerText()
{
	int winner{-1};
	int winnerPoints{-1};
	if (GameStateRef)
	{
		// Check remaining players
		for (int i = 0; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto player = GameStateRef->Server_Players[i])
			{
				if (player->Coins > winnerPoints)
				{
					winner = i;
					winnerPoints = player->Coins;
				}
			}
		}
	}

	// Set text of who won
	switch (winner)
	{
	case 0:
		{
			GameWinnerText->SetText(FText::FromString("Player 1 Wins"));
			break;
		}
	case 1:
		{
			GameWinnerText->SetText(FText::FromString("Player 2 Wins"));
			break;
		}
	case 2:
		{
			GameWinnerText->SetText(FText::FromString("Player 3 Wins"));
			break;
		}
	case 3:
		{
			GameWinnerText->SetText(FText::FromString("Player 4 Wins"));
			break;
		}
	default:
		{
			GameWinnerText->SetText(FText::FromString("Winner Error: No Winner"));
		break;
		}
	}
	
	// Update points
	GameWinnerPoints->SetText(FText::FromString(FString::FromInt(winnerPoints)));
}

void UWidget_EndgameMenu::EnableEndgameMenu()
{
	SetVisibility(ESlateVisibility::Visible);
	
	if (auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		controller->SetInputMode(FInputModeUIOnly{});
		controller->bShowMouseCursor = true;
	}
}
