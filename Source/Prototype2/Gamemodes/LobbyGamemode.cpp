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
		if (auto playerState = NewPlayer->GetPlayerState<ALobbyPlayerState>())
		{
			if (auto gamestate = GetGameState<ALobbyGamestate>())
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

				if (auto character = Cast<ALobbyCharacter>(NewPlayer->GetCharacter()))
				{
					character->SetPlayerState(playerState);
					if (PlayerMaterials.Num() > (int)playerState->Character * 3 + (int)playerState->CharacterColour)
					{
						character->PlayerMat = PlayerMaterials[(int)playerState->Character * 3 + (int)playerState->CharacterColour];
					}
					
					character->PlayerStateRef = playerState;
					
					NewPlayer->Possess(character);
					character->SetOwner(NewPlayer);
					gamestate->MaxPlayersOnServer = GetGameInstance<UPrototypeGameInstance>()->MaxPlayersOnServer;
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

void ALobbyGamemode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (auto gamestate = GetGameState<ALobbyGamestate>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Gamestate"));
		if (auto playerState = Exiting->GetPlayerState<ALobbyPlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Gamemode: Removing Player %s froom Server_Players"), *FString::FromInt(playerState->Player_ID));
			gamestate->Server_Players.Remove(playerState);
		}
	}
}

void ALobbyGamemode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (auto gamestate = GetGameState<ALobbyGamestate>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Gamestate"));
		for(auto i = 0; i < gamestate->Server_Players.Num(); i++)
		{
			if (auto playerState = gamestate->Server_Players[i])
			{
				if (auto controller = playerState->GetPlayerController())
				{
					//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Player Controller"));
					if (auto character = Cast<ALobbyCharacter>(controller->GetCharacter()))
					{
						//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Player Character"));
						if (PlayerMaterials.Num() > (int)playerState->CharacterColour)
						{
							//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Set Player Material"));
							if (PlayerMaterials.Num() > (int)playerState->Character * 4 + (int)playerState->CharacterColour)
								character->PlayerMat = PlayerMaterials[(int)playerState->Character * 4 + (int)playerState->CharacterColour];
							else
								character->PlayerMat = PlayerMaterials[(int)playerState->CharacterColour];
						}
					}
				}
			}
		}
	}
}


