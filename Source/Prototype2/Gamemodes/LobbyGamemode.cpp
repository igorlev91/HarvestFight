#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyCharacter.h"
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
				UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
				playerState->Player_ID = gamestate->Server_Players.Add(playerState);

				if (auto* character = Cast<ALobbyCharacter>(NewPlayer->GetCharacter()))
				{
					character->PlayerMat = PlayerMaterials[playerState->Player_ID];

					switch(playerState->Player_ID)
					{
					case 0:
						{
							character->SetActorLocation({567.373744f,-143.041463f,97.0});
							break;
						}
					case 1:
						{
							character->SetActorLocation({324.557804,-231.419228,97.0});
							break;
						}
					case 2:
						{
							character->SetActorLocation({99.813257,-313.219544,97.0});
							break;
						}
					case 3:
						{
							character->SetActorLocation({-122.669679,-394.196701,97.0});
							break;
						}
					default:
						character->SetActorLocation({567.373744f,-143.041463f,97.0});
						break;
					}
				}
				
				
			}
		}
	}
}


