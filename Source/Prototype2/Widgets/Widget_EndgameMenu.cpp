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

	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_EndgameMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateWinnerText();

}

void UWidget_EndgameMenu::UpdateWinnerText()
{

	// Getting highest score
	
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

	// Checking for draws
	bool IsP1{false};
	bool IsP2{false};
	bool IsP3{false};
	bool IsP4{false};
	
	if (GameStateRef)
	{
		// Check remaining players
		for (int i = 0; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto player = GameStateRef->Server_Players[i])
			{
				if (player->Coins == winnerPoints)
				{
					if (i == 0)
					{
						IsP1 = true;
					}
					else if (i == 1)
					{
						IsP2 = true;
					}
					else if (i == 2)
					{
						IsP3 = true;
					}
					else if (i == 3)
					{
						IsP4 = true;
					}
				}
			}
		}
	}

	// Update text
	if (IsP1 == true && IsP2 == false && IsP3 == false  && IsP4 == false) // P1 wins
	{
		GameWinnerText->SetText(FText::FromString("WINNER!"));
		GameWinnerText_1->SetText(FText::FromString("Player 1"));
	}
	else if (IsP1 == false && IsP2 == true && IsP3 == false  && IsP4 == false) // P2 Wins
	{
		GameWinnerText->SetText(FText::FromString("WINNER!"));
		GameWinnerText_1->SetText(FText::FromString("Player 2"));
	}
	else if (IsP1 == false && IsP2 == false && IsP3 == true  && IsP4 == false) // P3 Wins
	{
		GameWinnerText->SetText(FText::FromString("WINNER!"));
		GameWinnerText_1->SetText(FText::FromString("Player 3"));
	}
	else if (IsP1 == false && IsP2 == false && IsP3 == false  && IsP4 == true) // P4 Wins
	{
		GameWinnerText->SetText(FText::FromString("WINNER!"));
		GameWinnerText_1->SetText(FText::FromString("Player 4"));
	}
	else if (IsP1 == true && IsP2 == true && IsP3 == false  && IsP4 == false) // P1 & P2 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 1 & 2"));
	}
	else if (IsP1 == true && IsP2 == false && IsP3 == true  && IsP4 == false) // P1 & P3 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 1 & 3"));
	}
	else if (IsP1 == true && IsP2 == false && IsP3 == false  && IsP4 == true) // P1 & P4 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 1 & 4"));
	}
	else if (IsP1 == false && IsP2 == true && IsP3 == true  && IsP4 == false) // P2 & P3 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 2 & 3"));
	}
	else if (IsP1 == false && IsP2 == true && IsP3 == false  && IsP4 == true) // P2 & P4 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 2 & 4"));
	}
	else if (IsP1 == false && IsP2 == false && IsP3 == true  && IsP4 == true) // P3 & P4 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 3 & 4"));
	}
	else if (IsP1 == true && IsP2 == true && IsP3 == true  && IsP4 == false) // P1 & P2 & P3 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 1, 2 & 3"));
	}
	else if (IsP1 == true && IsP2 == true && IsP3 == false  && IsP4 == true) // P1 & P2 & P4 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 1, 2 & 4"));
	}
	else if (IsP1 == true && IsP2 == false && IsP3 == true  && IsP4 == true) // P1 & P3 & P4 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 1, 3 & 4"));
	}
	else if (IsP1 == false && IsP2 == true && IsP3 == true  && IsP4 == true) // P2 & P3 & P4 Draw
	{
		GameWinnerText->SetText(FText::FromString("Draw!"));
		GameWinnerText_1->SetText(FText::FromString("Players 2, 3 & 4"));
	}
	else
	{
		GameWinnerText->SetText(FText::FromString("4 Cow Draw!"));
		GameWinnerText_1->SetText(FText::FromString(""));
	}
		





	
	
	//// Set text of who won
	//switch (winner)
	//{
	//case 0:
	//	{
//
	//		GameWinnerText->SetText(FText::FromString("WINNER!"));
	//		GameWinnerText_1->SetText(FText::FromString("Player 1"));
//
	//		break;
	//	}
	//case 1:
	//	{
	//		GameWinnerText->SetText(FText::FromString("WINNER!"));
	//		GameWinnerText_1->SetText(FText::FromString("Player 2"));
	//		break;
	//	}
	//case 2:
	//	{
	//		GameWinnerText->SetText(FText::FromString("WINNER!"));
	//		GameWinnerText_1->SetText(FText::FromString("Player 3"));
	//		break;
	//	}
	//case 3:
	//	{
	//		GameWinnerText->SetText(FText::FromString("WINNER!"));
	//		GameWinnerText_1->SetText(FText::FromString("Player 4"));
	//		break;
	//	}
	//default:
	//	{
	//		GameWinnerText->SetText(FText::FromString("Winner Error: No Winner"));
	//	break;
	//	}
	//}
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
