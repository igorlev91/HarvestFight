// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyPlayerHUD.h"

#include <string>

#include "Widget_MapChoice.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyPlayerState.h"
#include "Prototype2/Prototype2PlayerController.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

void UWidget_LobbyPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gameState;
	}

	// Make cancel hidden
	CancelButton->SetVisibility(ESlateVisibility::Hidden);
	StartCountDown->SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_LobbyPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameStateRef)
	{
		Player1ReadyImage->SetVisibility(ESlateVisibility::Hidden);
		Player2ReadyImage->SetVisibility(ESlateVisibility::Hidden);
		Player3ReadyImage->SetVisibility(ESlateVisibility::Hidden);
		Player4ReadyImage->SetVisibility(ESlateVisibility::Hidden);

		if (GameStateRef->IsCountingDown)
		{
			StartCountDown->SetVisibility(ESlateVisibility::Visible);
			StartCountDown->SetText(FText::FromString(FString::FromInt(GameStateRef->LobbyLengthSeconds)));
		}
		else
		{
			StartCountDown->SetVisibility(ESlateVisibility::Hidden);
		}
		
		for(int i = 0; i < GameStateRef->Server_Players.Num(); i++)
		{
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->Server_Players[i]))
			{
				switch(i)
				{
				case 0:
					{
						if (playerstate->IsReady == true)
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
						if (playerstate->IsReady == true)
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
						if (playerstate->IsReady == true)
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
						if (playerstate->IsReady == true)
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
		// Show map choice
		if (GameStateRef->bShowMapChoice)
		{
			MapChoiceWidget->SetVisibility(ESlateVisibility::Visible);
			//MapChoiceWidget->EnableMapChoice(); // Show map choice UI
			
			MapChoiceWidget->Farm_Counter->SetText(FText::FromString(FString::FromInt(GameStateRef->Farm)));
			MapChoiceWidget->WinterFarm_Counter->SetText(FText::FromString(FString::FromInt(GameStateRef->WinterFarm)));

			if (GameStateRef->Farm > 0)
			{
				MapChoiceWidget->Farm_Counter->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				MapChoiceWidget->Farm_Counter->SetVisibility(ESlateVisibility::Hidden);
			}
			if (GameStateRef->WinterFarm > 0)
			{
				MapChoiceWidget->WinterFarm_Counter->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				MapChoiceWidget->WinterFarm_Counter->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UWidget_LobbyPlayerHUD::SetReady()
{
	if (auto* playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		auto playerID = playerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
		if (playerController->IsLocalPlayerController())
			UE_LOG(LogTemp, Warning, TEXT("Players ID = %s"), *FString::FromInt(playerID));
		
		if (GameStateRef->Server_Players.Num() >= playerID)
		{
			if (auto* playerState = Cast<ALobbyPlayerState>(GameStateRef->Server_Players[playerID]))
			{
				playerController->SetIsReady(playerID, true);

				ReadyButton->SetVisibility(ESlateVisibility::Hidden);
				CancelButton->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UWidget_LobbyPlayerHUD::SetCancel()
{
	if (auto* playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		auto playerID = playerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
		if (playerController->IsLocalPlayerController())
			UE_LOG(LogTemp, Warning, TEXT("Players ID = %s"), *FString::FromInt(playerID));
		
		if (GameStateRef->Server_Players.Num() >= playerID)
		{
			if (auto* playerState = Cast<ALobbyPlayerState>(GameStateRef->Server_Players[playerID]))
			{
				playerController->SetIsReady(playerID, false);

				ReadyButton->SetVisibility(ESlateVisibility::Visible);
				CancelButton->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

