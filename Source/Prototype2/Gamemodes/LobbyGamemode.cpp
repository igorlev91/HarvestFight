#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyPlayerState.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"


ALobbyGamemode::ALobbyGamemode()
{
	PrimaryActorTick.bCanEverTick = true;

	
}

void ALobbyGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (HasAuthority())
	{
		if (auto* playerState = NewPlayer->GetPlayerState<ALobbyPlayerState>())
		{
			if (auto* gamestate = GetGameState<ALobbyGamestate>())
			{
				gamestate->Server_Players.Add(playerState);
				playerState->Player_ID = gamestate->Server_Players.Num() - 1;
			}
		}
	}
}


