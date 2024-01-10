

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

	/* Check if only one player has won, or multiple (draw) */
	int32 WinnerCounter = 0;
	if (GameStateReference)
	{
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
	}

	/* Update text to say Draw */
	if (WinnerCounter > 1)
		TextGameWinner->SetText(FText::FromString("DRAW!"));
	
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
