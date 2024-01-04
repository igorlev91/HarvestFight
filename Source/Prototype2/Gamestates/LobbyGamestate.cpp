#include "LobbyGamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/LobbyCharacter.h"
#include "Prototype2/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Widgets/Widget_IngameMenu.h"
#include "Prototype2/Widgets/Widget_MapChoice.h"

ALobbyGamestate::ALobbyGamestate()
{
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = true;
}

void ALobbyGamestate::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	
	if (HasAuthority())
	{
		if (PreviousServerTravel != ShouldServerTravel)
		{
			PreviousServerTravel = ShouldServerTravel;

			if (ShouldServerTravel)
			{
				if (LobbyLengthSeconds > 0 || LobbyLengthMinutes > 0)
				{
					IsCountingDown = true;
				}
				PreviousServerTravel = false;
				ShouldServerTravel = false;
			}
		}

		if (IsCountingDown)
		{
			if (LobbyLengthSeconds > 0)
			{
				LobbyLengthSeconds -= DeltaSeconds;
			}
			else
			{
				if (LobbyLengthMinutes <= 0)
				{
					// Show map choice
					bShowMapChoice = true;
					
					//UE_LOG(LogTemp, Warning, TEXT("Farm: %d"), Farm);
					//UE_LOG(LogTemp, Warning, TEXT("WinterFarm: %d"), WinterFarm);

					int totalVotes = Farm + WinterFarm;
					if (totalVotes == Server_Players.Num())
					{
						bMapChosen = true; // Turned true so that it will change HUD visibility for timer
						if (Farm > WinterFarm)
						{
							MapChoice = "Level_Main";
						}
						else if (WinterFarm > Farm)
						{
							MapChoice = "Level_Winter";
						}
						else
						{
							int randomNum = FMath::RandRange(0, 1);
							if (randomNum == 0)
							{
								MapChoice = "Level_Main";
							}
							else
							{
								MapChoice = "Level_Winter";
							}
						}

						// Countdown between all players choosing map and actually starting
						MapChoiceLengthSeconds -= DeltaSeconds;
						if (MapChoiceLengthSeconds <= 0)
						{
							IsCountingDown = false;
							if (auto gamestate = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
							{
								if (auto gameInstance = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
								{
									gameInstance->FinalConnectionCount = gamestate->Server_Players.Num();
								}
							}
							GetWorld()->ServerTravel(MapChoice, false, false); // Start level
						}
					}
				}
				else
				{
					LobbyLengthMinutes--;
					LobbyLengthSeconds = 60;
				}
			}
		}
	}
}

void ALobbyGamestate::SetIsReady(int _player, bool _isReady)
{
	if (Server_Players.Num() >= _player)
	{
		if (auto playerState = Server_Players[_player])
		{
			playerState->SetIsReady(_isReady);
		}
	}

	bool isEveryoneReady{true};
	for(auto player : Server_Players)
	{
		if (!player->IsReady)
			isEveryoneReady = false;
	}
	
	if (isEveryoneReady && Server_Players.Num() >= 1)
	{
		ShouldServerTravel = true;
		LobbyLengthSeconds = 2.0f;
	}
	else
	{
		ShouldServerTravel = false;
		IsCountingDown = false;
		PreviousServerTravel = false;
		LobbyLengthSeconds = 2.0f;
	}
}

void ALobbyGamestate::VoteMap(int _player, EFarm _level)
{
	if (_level == EFarm::FARM)
	{
		Farm = Farm + 1;
	}
	else if (_level == EFarm::WINTERFARM)
	{
		WinterFarm = WinterFarm + 1;
	}
}

void ALobbyGamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthMinutes);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, IsCountingDown);
	DOREPLIFETIME(ALobbyGamestate, PreviousServerTravel);
	DOREPLIFETIME(ALobbyGamestate, Server_Players);
	DOREPLIFETIME(ALobbyGamestate, bShowMapChoice);

	DOREPLIFETIME(ALobbyGamestate, Farm);
	DOREPLIFETIME(ALobbyGamestate, WinterFarm);

	DOREPLIFETIME(ALobbyGamestate, MapChoiceLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, bMapChosen);
	
	DOREPLIFETIME(ALobbyGamestate, MaxPlayersOnServer);
}

void ALobbyGamestate::UpdateCharacterMaterial(int _player, ECharacters _character, ECharacterColours _characterColour)
{
	if (Server_Players.Num() >= _player)
	{
		if (auto playerState = Server_Players[_player])
		{
			playerState->UpdateCharacterMaterial(_character, _characterColour);
		}
	}
}

int ALobbyGamestate::GetNumberOfCharactersTaken(ECharacters _desiredCharacter)
{
	int characterCount{};
	if (Server_Players.Num() > 0)
	{
		for(auto playerState : Server_Players)
		{
			if (playerState.Get())
			{
				if (playerState->Character == _desiredCharacter)
					characterCount++;
			}
		}
	}
	return characterCount;
}

int ALobbyGamestate::GetNumberOfCharacterColoursTaken(ECharacters _desiredCharacter, ECharacterColours _desiredColour)
{
	int characterColourCount{};
	if (Server_Players.Num() > 0)
	{
		for(auto playerState : Server_Players)
		{
			if (playerState.Get())
			{
				if (playerState->Character == _desiredCharacter && playerState->CharacterColour == _desiredColour)
					characterColourCount++;
			}
		}
	}
	
	return characterColourCount;
}


