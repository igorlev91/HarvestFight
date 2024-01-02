// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_MapChoice.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Prototype2/LobbyPlayerState.h"

void UWidget_MapChoice::EnableMapChoice()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UWidget_MapChoice::SelectLevel(EFarm _level)
{
	//if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	//{
	//	gameState->MapChoice = _level;
	//	gameState->bMapChosen = true;
	//}
	if (bMapSelected == false)
	{
		bMapSelected = true;

		if (auto* playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
		{
			auto playerID = playerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
			playerController->VoteMap(playerID, _level);
		}
		
		//if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
		//{
		//	if (_level == EFarm::FARM)
		//	{
		//		gameState->Farm = gameState->Farm + 1;
		//	}
		//	else if (_level == EFarm::WINTERFARM)
		//	{
		//		gameState->WinterFarm = gameState->WinterFarm + 1;
		//	}
		//}
	}
}


