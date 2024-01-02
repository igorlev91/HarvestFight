// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_MapChoice.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

void UWidget_MapChoice::EnableMapChoice()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UWidget_MapChoice::SelectLevel(FString _level)
{
	if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		gameState->MapChoice = _level;
		gameState->bMapChosen = true;
	}
}

