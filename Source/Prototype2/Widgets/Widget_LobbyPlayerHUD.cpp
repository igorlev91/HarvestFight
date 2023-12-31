// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyPlayerHUD.h"

#include <string>

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
}

void UWidget_LobbyPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameStateRef)
	{
		Player1ReadyText->SetVisibility(ESlateVisibility::Hidden);
		Player2ReadyText->SetVisibility(ESlateVisibility::Hidden);
		Player3ReadyText->SetVisibility(ESlateVisibility::Hidden);
		Player4ReadyText->SetVisibility(ESlateVisibility::Hidden);
		Player1Text->SetVisibility(ESlateVisibility::Hidden);
		Player2Text->SetVisibility(ESlateVisibility::Hidden);
		Player3Text->SetVisibility(ESlateVisibility::Hidden);
		Player4Text->SetVisibility(ESlateVisibility::Hidden);
		
		if (GameStateRef->Server_Players.Num() >= 1)
			Player1Text->SetVisibility(ESlateVisibility::Visible);
		if (GameStateRef->Server_Players.Num() >= 2)
			Player2Text->SetVisibility(ESlateVisibility::Visible);
		if (GameStateRef->Server_Players.Num() >= 3)
			Player3Text->SetVisibility(ESlateVisibility::Visible);
		if (GameStateRef->Server_Players.Num() >= 4)
			Player4Text->SetVisibility(ESlateVisibility::Visible);
		
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
							Player1ReadyText->SetText(FText::FromString("Ready"));
							Player1ReadyText->SetVisibility(ESlateVisibility::Visible);
						}
						else
						{
							Player1ReadyText->SetText(FText::FromString("Not Ready"));
						}
						break;
					}
				case 1:
					{
						if (playerstate->IsReady == true)
						{
							Player2ReadyText->SetText(FText::FromString("Ready"));
							Player2ReadyText->SetVisibility(ESlateVisibility::Visible);
						}
						else
						{
							Player2ReadyText->SetText(FText::FromString("Not Ready"));
						}
						break;
					}
				case 2:
					{
						if (playerstate->IsReady == true)
						{
							Player3ReadyText->SetText(FText::FromString("Ready"));
							Player3ReadyText->SetVisibility(ESlateVisibility::Visible);
						}
						else
						{
							Player3ReadyText->SetText(FText::FromString("Not Ready"));
						}
						break;
					}
				case 3:
					{
						if (playerstate->IsReady == true)
						{
							Player4ReadyText->SetText(FText::FromString("Ready"));
							Player4ReadyText->SetVisibility(ESlateVisibility::Visible);
						}
						else
						{
							Player4ReadyText->SetText(FText::FromString("Not Ready"));
						}
						break;
					}
				default:
					break;
				}
					
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
				if (playerState->IsReady == true)
				{
					playerController->SetIsReady(playerID, false);
					IsReadyButtonText->SetText(FText::FromString("Ready"));

				}
				else
				{
					playerController->SetIsReady(playerID, true);
					IsReadyButtonText->SetText(FText::FromString("Not Ready"));
				}
			}
		}
	}
	
}
