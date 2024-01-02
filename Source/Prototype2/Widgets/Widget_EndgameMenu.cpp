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


}

void UWidget_EndgameMenu::UpdateWinnerText()
{
	if (GameStateRef)
	{
		// Set first player as winner
		Winner = 0;
		if (auto player = GameStateRef->Server_Players[0])
		{
			if (auto* playerState = Cast<APrototype2PlayerState>(player))
			{
				WinnerPoints = playerState->Coins;
			}
		}

		// Check remaining players
		for (int i = 1; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto player = GameStateRef->Server_Players[i])
			{
				if (auto* playerState = Cast<APrototype2PlayerState>(player))
				{
					if (playerState->Coins > WinnerPoints)
					{
						Winner = i;
						WinnerPoints = playerState->Coins;
					}
				}
			}
		}
	}

	// Set text of who won
	if (Winner == 0)
	{
		GameWinnerText->SetText(FText::FromString("Player 1 Wins"));
	}
	else if (Winner == 1)
	{
		GameWinnerText->SetText(FText::FromString("Player 2 Wins"));
	}
	else if (Winner == 2)
	{
		GameWinnerText->SetText(FText::FromString("Player 3 Wins"));
	}
	else
	{
		GameWinnerText->SetText(FText::FromString("Player 4 Wins"));
	}
	
	// Update points
	GameWinnerPoints->SetText(FText::FromString(FString::FromInt(WinnerPoints)));

	
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
