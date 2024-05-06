

#include "Widget_EndgameMenu.h"

#include "Widget_MapChoice.h"
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

	EnableEndgameMapChoice();
}

void UWidget_EndgameMenu::UpdateWinnerText()
{
	if (!GameStateReference)
		return;

	if (GameStateReference->bTeams)
	{
		int32 Team1Coins{};
		int32 Team2Coins{};
		if (!GameStateReference->PlayerArray.IsEmpty())
		{
			for(auto PlayerState : GameStateReference->PlayerArray)
			{
				if (PlayerState)
				{
					if (APrototype2PlayerState* CastedPlayerstate = Cast<APrototype2PlayerState>(PlayerState))
					{
						if (CastedPlayerstate->Details.Colour == GameStateReference->TeamOneColour)
						{
							Team1Coins += CastedPlayerstate->Coins;
						}
						else if (CastedPlayerstate->Details.Colour == GameStateReference->TeamTwoColour)
						{
							Team2Coins += CastedPlayerstate->Coins;
						}
					}
				}
			}
		}
		FString TeamName;
		int LastFoundSIndex{};
		GameStateReference->TeamOneName.FindLastChar('s', LastFoundSIndex);
		
		if (Team1Coins == Team2Coins)
			TextGameWinner->SetText(FText::FromString("DRAW!"));
		else if (Team1Coins > Team2Coins)
		{
			if (LastFoundSIndex != GameStateReference->TeamOneName.Len() - 1)
				TeamName = GameStateReference->TeamOneName + "'s Win!";
			else
				TeamName = GameStateReference->TeamOneName + " Win!";
			
			TextGameWinner->SetColorAndOpacity(FLinearColor(
				GameStateReference->Server_TeamOne[0]->Details.PureToneColour.X, 
				GameStateReference->Server_TeamOne[0]->Details.PureToneColour.Y,
				GameStateReference->Server_TeamOne[0]->Details.PureToneColour.Z,
				1));

			//if (GameStateReference->TeamOneColour == EColours::WHITE)
			//{
			//	FSlateFontInfo FontInfo{};
			//	FontInfo = TextGameWinner->GetFont();
			//	FontInfo.OutlineSettings = TextGameWinner->GetFont().OutlineSettings;
			//	FontInfo.OutlineSettings.OutlineColor = FLinearColor::Black;
			//}
		}
		else
		{
			if (LastFoundSIndex != GameStateReference->TeamOneName.Len() - 1)
				TeamName = GameStateReference->TeamTwoName + "'s Win!";
			else
				TeamName = GameStateReference->TeamTwoName + " Win!";

			TextGameWinner->SetColorAndOpacity(FLinearColor(
				GameStateReference->Server_TeamTwo[0]->Details.PureToneColour.X, 
				GameStateReference->Server_TeamTwo[0]->Details.PureToneColour.Y,
				GameStateReference->Server_TeamTwo[0]->Details.PureToneColour.Z,
				1));
			

			//if (GameStateReference->TeamTwoColour == EColours::WHITE)
			//{
			//	FSlateFontInfo FontInfo{};
			//	FontInfo = TextGameWinner->GetFont();
			//	FontInfo.OutlineSettings = TextGameWinner->GetFont().OutlineSettings;
			//	FontInfo.OutlineSettings.OutlineColor = FLinearColor::Black;
			//}
		}
		TextGameWinner->SetText(FText::FromString(TeamName));
	}
	else
	{
		/* Calculating highest score */
		//int WinningScore{-1};
		//for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
		//{
		//	if (auto Player = GameStateReference->Server_Players[i])
		//	{
		//		if (Player->Coins > WinningScore)
		//		{
		//			WinningScore = Player->Coins;
		//		}
		//	}
		//}
//
		///* Check if only one player has won, or multiple (draw) */
		//int32 WinnerCounter = 0;
		///* Loop through players and see if they have same score as winning score */
		//for (int i = 0; i < GameStateReference->Server_Players.Num(); i++)
		//{
		//	if (auto Player = GameStateReference->Server_Players[i])
		//	{
		//		if (Player->Coins == WinningScore)
		//		{
		//			WinnerCounter++;
		//		}
		//	}
		//}

		/* Update text to say Draw */
		//if (WinnerCounter > 1)
		//	TextGameWinner->SetText(FText::FromString("DRAW!"));
	}
}

void UWidget_EndgameMenu::EnableEndgameMenu()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
		
		/*if (auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			
			//PlayerController->SetInputMode(FInputModeUIOnly{});
			PlayerController->bShowMouseCursor = true;
		}*/
	}
}

void UWidget_EndgameMenu::EnableEndgameMapChoice()
{
	if (!GameStateReference)
		return;
	
	if (WBP_MapChoice->GetVisibility() == ESlateVisibility::Visible)
		return;
	
	if (WBP_MapChoiceBrawl->GetVisibility() == ESlateVisibility::Visible)
		return;
	
	if (WBP_MapChoiceBlitz->GetVisibility() == ESlateVisibility::Visible)
		return;

	if (GameStateReference->bIsMapChoiceShowing == false)
		return;
	
	if (GameStateReference->GameMode == 0)
		WBP_MapChoice->SetVisibility(ESlateVisibility::Visible);
	else if (GameStateReference->GameMode == 1)
		WBP_MapChoiceBrawl->SetVisibility(ESlateVisibility::Visible);
	else
		WBP_MapChoiceBlitz->SetVisibility(ESlateVisibility::Visible);
}

void UWidget_EndgameMenu::ShowLoadingScreen(UUserWidget* Widget)
{
	UPrototypeGameInstance* GameInstance = GetGameInstance<UPrototypeGameInstance>();

	if (GameInstance)
		GameInstance->ShowLoadingScreen(Widget, 0);
}


