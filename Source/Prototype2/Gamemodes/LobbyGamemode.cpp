#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyCharacter.h"
#include "Prototype2/LobbyPlayerState.h"
#include "Prototype2/Prototype2Character.h"
#include "Prototype2/PrototypeGameInstance.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"


ALobbyGamemode::ALobbyGamemode()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Player"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
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
				playerState->CharacterColour = (ECharacterColours)((rand() % 3) + 1);
				int numOfPlayersWithSameColour{2};
				while(numOfPlayersWithSameColour >= 1)
				{
					numOfPlayersWithSameColour = 0;
					for(auto otherPlayerState : gamestate->Server_Players)
					{
						if (otherPlayerState->CharacterColour == playerState->CharacterColour)
						{
							numOfPlayersWithSameColour++;
						}
					}
					if (numOfPlayersWithSameColour >= 1)
					{
						int newColour = (int)playerState->CharacterColour;
						newColour ++;
						if (newColour > 3)
						{
							newColour = 0;
						}
						else if (newColour < 0)
						{
							newColour = 3;
						}
						playerState->CharacterColour = (ECharacterColours)newColour;
					}
				}
				
				//UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
				playerState->Player_ID = gamestate->Server_Players.Add(playerState);

				if (auto* character = Cast<ALobbyCharacter>(NewPlayer->GetCharacter()))
				{
					character->PlayerMat = PlayerMaterials[(int)playerState->CharacterColour];
					NewPlayer->Possess(character);
					character->SetOwner(NewPlayer);
					gamestate->MaxPlayersOnServer = GetGameInstance<UPrototypeGameInstance>()->MaxPlayersOnServer;
					
					//UE_LOG(LogTemp, Warning, TEXT("Public Connection Count (Lobby): %s"), *FString::FromInt(gamestate->MaxPlayersOnServer));
					switch(playerState->Player_ID)
					{
					case 0:
						{
							if (gamestate->MaxPlayersOnServer == 4)
							{
								character->SetActorLocation({Position1});
							}
							else if (gamestate->MaxPlayersOnServer == 3)
							{
								character->SetActorLocation({Position2});
							}
							else
							{
								character->SetActorLocation({Position3});
							}
							break;
						}
					case 1:
						{
							if (gamestate->MaxPlayersOnServer == 4)
							{
								character->SetActorLocation({Position3});
							}
							else if (gamestate->MaxPlayersOnServer == 3)
							{
								character->SetActorLocation({Position4});
							}
							else
							{
								character->SetActorLocation({Position5});
							}
							break;
						}
					case 2:
						{
							if (gamestate->MaxPlayersOnServer == 4)
							{
								character->SetActorLocation({Position5});
							}
							else
							{
								character->SetActorLocation({Position6});
							}
							break;
						}
					case 3:
						{
							character->SetActorLocation({Position7});
							break;
						}
					default:
						
						break;
					}
				}
			}
		}
	}
}

void ALobbyGamemode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (auto gamestate = GetGameState<ALobbyGamestate>())
	{
		for(auto i = 0; i < gamestate->Server_Players.Num(); i++)
		{
			if (auto playerState = gamestate->Server_Players[i])
			{
				if (auto controller = playerState->GetPlayerController())
				{
					if (auto character = Cast<ALobbyCharacter>(controller->GetCharacter()))
					{
						if (PlayerMaterials.Num() > (int)playerState->CharacterColour)
							character->PlayerMat = PlayerMaterials[(int)playerState->CharacterColour];
					}
				}
			}
		}
	}
}


