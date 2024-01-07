

#include "Widget_LobbyPlayerHUD.h"

#include <string>

#include "Widget_MapChoice.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

void UWidget_LobbyPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}

	// Make cancel hidden
	CancelButton->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_LobbyPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GameStateReference)
	{
		return;
	}
	
	/* Preset ready images to being Hidden */
	Player1ReadyImage->SetVisibility(ESlateVisibility::Hidden);
	Player2ReadyImage->SetVisibility(ESlateVisibility::Hidden);
	Player3ReadyImage->SetVisibility(ESlateVisibility::Hidden);
	Player4ReadyImage->SetVisibility(ESlateVisibility::Hidden);
	
	for(int i = 0; i < GameStateReference->Server_Players.Num(); i++)
	{
		if (auto* PlayerState = Cast<ALobbyPlayerState>(GameStateReference->Server_Players[i]))
		{
			switch(i)
			{
			case 0:
				{
					if (PlayerState->IsReady == true)
					{
						Player1ReadyImage->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						Player1ReadyImage->SetVisibility(ESlateVisibility::Hidden);
					}
					break;
				}
			case 1:
				{
					if (PlayerState->IsReady == true)
					{
						Player2ReadyImage->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						Player2ReadyImage->SetVisibility(ESlateVisibility::Hidden);
					}
					break;
				}
			case 2:
				{
					if (PlayerState->IsReady == true)
					{
						Player3ReadyImage->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						Player3ReadyImage->SetVisibility(ESlateVisibility::Hidden);
					}
					break;
				}
			case 3:
				{
					if (PlayerState->IsReady == true)
					{
						Player4ReadyImage->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						Player4ReadyImage->SetVisibility(ESlateVisibility::Hidden);
					}
					break;
				}
			default:
				break;
			}
		}
	}
	/* Showing map choice widget */
	if (GameStateReference->ShouldShowMapChoices())
	{
		MapChoiceWidget->SetVisibility(ESlateVisibility::Visible); // Turn widget on for map choice
		
		MapChoiceWidget->Farm_Counter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetFarm()))); // Increase vote counter for map
		MapChoiceWidget->WinterFarm_Counter->SetText(FText::FromString(FString::FromInt(GameStateReference->GetWinterFarm()))); // Increase vote counter for map
		if (GameStateReference->GetFarm() > 0)
		{
			MapChoiceWidget->Farm_Counter->SetVisibility(ESlateVisibility::Visible); // Turns on visibility of vote text only if not 0
		}
		else
		{
			MapChoiceWidget->Farm_Counter->SetVisibility(ESlateVisibility::Hidden);
		}
		if (GameStateReference->GetWinterFarm() > 0)
		{
			MapChoiceWidget->WinterFarm_Counter->SetVisibility(ESlateVisibility::Visible); // Turns on visibility of vote text only if not 0
		}
		else
		{
			MapChoiceWidget->WinterFarm_Counter->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	/* Show timer after map vote */
	if (GameStateReference->HasMapBeenChosen())
	{
		MapChoiceWidget->MapChoiceTimer->SetVisibility(ESlateVisibility::Visible);
		MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString::FromInt(GameStateReference->GetMapChoiceLengthSeconds())));
	}
	if (GameStateReference->GetMapChoiceLengthSeconds() <= 0)
	{
		MapChoiceWidget->MapChoiceTimer->SetText(FText::FromString(FString("LOADING LEVEL...")));
	}
}

void UWidget_LobbyPlayerHUD::SetReady()
{
	if (auto* PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		auto PlayerID = PlayerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
		
		if (GameStateReference->Server_Players.Num() >= PlayerID)
		{
			if (auto* playerState = Cast<ALobbyPlayerState>(GameStateReference->Server_Players[PlayerID]))
			{
				PlayerController->SetIsReady(PlayerID, true);

				ReadyButton->SetVisibility(ESlateVisibility::Hidden);
				CancelButton->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UWidget_LobbyPlayerHUD::SetCancel()
{
	if (auto* PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		auto PlayerID = PlayerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
		
		if (GameStateReference->Server_Players.Num() >= PlayerID)
		{
			if (auto* playerState = Cast<ALobbyPlayerState>(GameStateReference->Server_Players[PlayerID]))
			{
				PlayerController->SetIsReady(PlayerID, false);

				ReadyButton->SetVisibility(ESlateVisibility::Visible);
				CancelButton->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

