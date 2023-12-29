// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyPlayerHUD.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyPlayerState.h"
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
		if (GameStateRef->PlayerArray.Num() == 2)
		{
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[0]))
			{
				if (playerstate->IsReady == true)
				{
					Player1ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player1ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[1]))
			{
				if (playerstate->IsReady == true)
				{
					Player2ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player2ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}

			// Turn off other text
			Player2Text->SetVisibility(ESlateVisibility::Visible);
			Player2ReadyText->SetVisibility(ESlateVisibility::Visible);
			Player3Text->SetVisibility(ESlateVisibility::Hidden);
			Player3ReadyText->SetVisibility(ESlateVisibility::Hidden);
			Player4Text->SetVisibility(ESlateVisibility::Hidden);
			Player4ReadyText->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (GameStateRef->PlayerArray.Num() == 3)
		{
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[0]))
			{
				if (playerstate->IsReady == true)
				{
					Player1ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player1ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[1]))
			{
				if (playerstate->IsReady == true)
				{
					Player2ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player2ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[2]))
			{
				if (playerstate->IsReady == true)
				{
					Player2ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player2ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}

			// Turn off other text
			Player2Text->SetVisibility(ESlateVisibility::Visible);
			Player2ReadyText->SetVisibility(ESlateVisibility::Visible);
			Player3Text->SetVisibility(ESlateVisibility::Visible);
			Player3ReadyText->SetVisibility(ESlateVisibility::Visible);
			Player4Text->SetVisibility(ESlateVisibility::Hidden);
			Player4ReadyText->SetVisibility(ESlateVisibility::Hidden);
			
		}
		else if (GameStateRef->PlayerArray.Num() == 4)
		{
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[0]))
			{
				if (playerstate->IsReady == true)
				{
					Player1ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player1ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[1]))
			{
				if (playerstate->IsReady == true)
				{
					Player2ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player2ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[2]))
			{
				if (playerstate->IsReady == true)
				{
					Player2ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player2ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[3]))
			{
				if (playerstate->IsReady == true)
				{
					Player2ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player2ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}

			// Turn off other text
			Player2Text->SetVisibility(ESlateVisibility::Visible);
			Player2ReadyText->SetVisibility(ESlateVisibility::Visible);
			Player3Text->SetVisibility(ESlateVisibility::Visible);
			Player3ReadyText->SetVisibility(ESlateVisibility::Visible);
			Player4Text->SetVisibility(ESlateVisibility::Visible);
			Player4ReadyText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			if (auto* playerstate = Cast<ALobbyPlayerState>(GameStateRef->PlayerArray[0]))
			{
				if (playerstate->IsReady == true)
				{
					Player1ReadyText->SetText(FText::FromString("Ready"));
				}
				else
				{
					Player1ReadyText->SetText(FText::FromString("Not Ready"));
				}
			}
			// Turn off other text
			Player2Text->SetVisibility(ESlateVisibility::Hidden);
			Player2ReadyText->SetVisibility(ESlateVisibility::Hidden);
			Player3Text->SetVisibility(ESlateVisibility::Hidden);
			Player3ReadyText->SetVisibility(ESlateVisibility::Hidden);
			Player4Text->SetVisibility(ESlateVisibility::Hidden);
			Player4ReadyText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWidget_LobbyPlayerHUD::SetReady()
{
	if (auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (controller->GetPlayerState<ALobbyPlayerState>()->IsReady == true)
		{
			controller->GetPlayerState<ALobbyPlayerState>()->SetIsReady(false);
			IsReadyButtonText->SetText(FText::FromString("Ready"));

		}
		else
		{
			controller->GetPlayerState<ALobbyPlayerState>()->SetIsReady(true);
			IsReadyButtonText->SetText(FText::FromString("Not Ready"));
		}
	}
}
