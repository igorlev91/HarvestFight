

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
	if (!GameStateReference)
		return;

	if (GameStateReference->bTeams)
	{
		int32 RedTeamCoins{};
		int32 BlueTeamCoins{};
		bool RedTeamWon{};
		bool Draw{};
		for(auto PlayerState : GameStateReference->PlayerArray)
		{
			if (APrototype2PlayerState* CastedPlayerstate = Cast<APrototype2PlayerState>(PlayerState))
			{
				if (CastedPlayerstate->Details.Colour == EColours::RED)
				{
					RedTeamCoins += CastedPlayerstate->Coins;
				}
				else if (CastedPlayerstate->Details.Colour == EColours::BLUE)
				{
					BlueTeamCoins += CastedPlayerstate->Coins;
				}
			}
		}

		if (RedTeamCoins == BlueTeamCoins)
			TextGameWinner->SetText(FText::FromString("DRAW!"));
		else if (RedTeamCoins > BlueTeamCoins)
			TextGameWinner->SetText(FText::FromString("RED TEAM WINS!"));
		else
			TextGameWinner->SetText(FText::FromString("BLUE TEAM WINS!"));
	}
	else
	{
		/* Calculating highest score */
		int WinningScore{-1};
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

		/* Check if only one player has won, or multiple (draw) */
		int32 WinnerCounter = 0;
		/* Loop through players and see if they have same score as winning score */
		for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
		{
			if (auto Player = GameStateReference->Server_Players[i])
			{
				if (Player->Coins == WinningScore)
				{
					WinnerCounter++;
				}
			}
		}

		/* Update text to say Draw */
		if (WinnerCounter > 1)
			TextGameWinner->SetText(FText::FromString("DRAW!"));
	}
}

void UWidget_EndgameMenu::EnableEndgameMenu()
{
	if (GetVisibility() != ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Visible);
	
		/*if (auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			
			//PlayerController->SetInputMode(FInputModeUIOnly{});
			PlayerController->bShowMouseCursor = true;
		}*/
	}
}
