// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2GameMode.h"

#include "Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

class ALobbyGamestate;

APrototype2GameMode::APrototype2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}

void APrototype2GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void APrototype2GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (HasAuthority())
	{
		if (auto* playerState = NewPlayer->GetPlayerState<APrototype2PlayerState>())
		{
			if (auto* gamestate = GetGameState<APrototype2Gamestate>())
			{
				gamestate->Server_Players.Add(playerState);
				playerState->Player_ID = gamestate->Server_Players.Num() - 1;
			}
		}
	}
}


