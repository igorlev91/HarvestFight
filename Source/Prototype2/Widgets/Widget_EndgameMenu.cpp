

#include "Widget_EndgameMenu.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

void UWidget_EndgameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = gameState;
	}

	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_EndgameMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateWinnerText();
}

void UWidget_EndgameMenu::UpdateWinnerText()
{
	/* Calculating highest score */
	int WinningScore{-1};
	if (GameStateReference)
	{
		// Check remaining players
		for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
		{
			if (auto Player = GameStateReference->Server_Players[i])
			{
				if (Player->Coins > WinningScore)
				{
					WinningScore = Player->Coins;
				}
			}
		}
	}

	/* Checking for draws */
	bool bHasP1Won{false}; // P1
	bool bHasP2Won{false}; // P2
	bool bHasP3Won{false}; // P3
	bool bHasP4Won{false}; // P4
	
	if (GameStateReference)
	{
		/* Loop through players and see if they have same score as winning score */
		for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
		{
			if (auto player = GameStateReference->Server_Players[i])
			{
				if (player->Coins == WinningScore)
				{
					if (i == 0)
					{
						bHasP1Won = true;
					}
					else if (i == 1)
					{
						bHasP2Won = true;
					}
					else if (i == 2)
					{
						bHasP3Won = true;
					}
					else if (i == 3)
					{
						bHasP4Won = true;
					}
				}
			}
		}
	}

	// Update text
	if (bHasP1Won == true && bHasP2Won == false && bHasP3Won == false  && bHasP4Won == false) // P1 wins
	{
		TextGameWinner->SetText(FText::FromString("WINNER!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Player 1"));
	}
	else if (bHasP1Won == false && bHasP2Won == true && bHasP3Won == false  && bHasP4Won == false) // P2 Wins
	{
		TextGameWinner->SetText(FText::FromString("WINNER!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Player 2"));
	}
	else if (bHasP1Won == false && bHasP2Won == false && bHasP3Won == true  && bHasP4Won == false) // P3 Wins
	{
		TextGameWinner->SetText(FText::FromString("WINNER!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Player 3"));
	}
	else if (bHasP1Won == false && bHasP2Won == false && bHasP3Won == false  && bHasP4Won == true) // P4 Wins
	{
		TextGameWinner->SetText(FText::FromString("WINNER!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Player 4"));
	}
	else if (bHasP1Won == true && bHasP2Won == true && bHasP3Won == false  && bHasP4Won == false) // P1 & P2 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 1 & 2"));
	}
	else if (bHasP1Won == true && bHasP2Won == false && bHasP3Won == true  && bHasP4Won == false) // P1 & P3 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 1 & 3"));
	}
	else if (bHasP1Won == true && bHasP2Won == false && bHasP3Won == false  && bHasP4Won == true) // P1 & P4 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 1 & 4"));
	}
	else if (bHasP1Won == false && bHasP2Won == true && bHasP3Won == true  && bHasP4Won == false) // P2 & P3 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 2 & 3"));
	}
	else if (bHasP1Won == false && bHasP2Won == true && bHasP3Won == false  && bHasP4Won == true) // P2 & P4 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 2 & 4"));
	}
	else if (bHasP1Won == false && bHasP2Won == false && bHasP3Won == true  && bHasP4Won == true) // P3 & P4 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 3 & 4"));
	}
	else if (bHasP1Won == true && bHasP2Won == true && bHasP3Won == true  && bHasP4Won == false) // P1 & P2 & P3 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 1, 2 & 3"));
	}
	else if (bHasP1Won == true && bHasP2Won == true && bHasP3Won == false  && bHasP4Won == true) // P1 & P2 & P4 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 1, 2 & 4"));
	}
	else if (bHasP1Won == true && bHasP2Won == false && bHasP3Won == true  && bHasP4Won == true) // P1 & P3 & P4 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 1, 3 & 4"));
	}
	else if (bHasP1Won == false && bHasP2Won == true && bHasP3Won == true  && bHasP4Won == true) // P2 & P3 & P4 Draw
	{
		TextGameWinner->SetText(FText::FromString("Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString("Players 2, 3 & 4"));
	}
	else
	{
		TextGameWinner->SetText(FText::FromString("4 Cow Draw!"));
		TextGameWinnerPlayer->SetText(FText::FromString(""));
	}
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
