#include "LobbyGamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
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

void ALobbyGamestate::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	if (HasAuthority())
	{
		if (bPreviousServerTravel != bShouldServerTravel)
		{
			bPreviousServerTravel = bShouldServerTravel;

			if (bShouldServerTravel)
			{
				if (LobbyLengthSeconds > 0 || LobbyLengthMinutes > 0)
				{
					bIsCountingDown = true;
				}
				bPreviousServerTravel = false;
				bShouldServerTravel = false;
			}
		}

		if (bIsCountingDown)
		{
			if (LobbyLengthSeconds > 0)
			{
				LobbyLengthSeconds -= _DeltaSeconds;
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
						MapChoiceLengthSeconds -= _DeltaSeconds;
						if (MapChoiceLengthSeconds <= 0)
						{
							bIsCountingDown = false;
							if (auto gamestate = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
							{
								if (auto gameInstance = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
								{
									gameInstance->FinalConnectionCount = gamestate->Server_Players.Num();

									for(auto player : Server_Players)
									{
										IOnlineIdentityPtr IdentityInterface = IOnlineSubsystem::Get()->GetIdentityInterface();
										if (IdentityInterface.IsValid())
										{
											gameInstance->FinalPlayerNames.Add(IdentityInterface->GetPlayerNickname(player->Player_ID));
										}
										
										gameInstance->FinalCharacters.Add(player->Character);
										gameInstance->FinalColours.Add(player->CharacterColour);
									}
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

void ALobbyGamestate::SetIsReady(int32 _Player, bool _bIsReady)
{
	if (Server_Players.Num() >= _Player)
	{
		if (auto playerState = Server_Players[_Player])
		{
			playerState->SetIsReady(_bIsReady);
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
		bShouldServerTravel = true;
		LobbyLengthSeconds = 2.0f;
	}
	else
	{
		bShouldServerTravel = false;
		bIsCountingDown = false;
		bPreviousServerTravel = false;
		LobbyLengthSeconds = 2.0f;
	}
}

void ALobbyGamestate::VoteMap(EFarm _Level)
{
	if (_Level == EFarm::FARM)
	{
		Farm = Farm + 1;
	}
	else if (_Level == EFarm::WINTERFARM)
	{
		WinterFarm = WinterFarm + 1;
	}
}

void ALobbyGamestate::SetMaxPlayersOnServer(int32 _maxPlayersOnServer)
{
	MaxPlayersOnServer = _maxPlayersOnServer;
}

int32 ALobbyGamestate::GetMaxPlayersOnServer() const
{
	return MaxPlayersOnServer;
}

bool ALobbyGamestate::ShouldShowMapChoices() const
{
	return bShowMapChoice;
}

int32 ALobbyGamestate::GetFarm() const
{
	return Farm;
}

int32 ALobbyGamestate::GetWinterFarm() const
{
	return WinterFarm;
}

bool ALobbyGamestate::HasMapBeenChosen() const
{
	return bMapChosen;
}

int32 ALobbyGamestate::GetMapChoiceLengthSeconds() const
{
	return MapChoiceLengthSeconds;
}

void ALobbyGamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthMinutes);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, bIsCountingDown);
	DOREPLIFETIME(ALobbyGamestate, bPreviousServerTravel);
	DOREPLIFETIME(ALobbyGamestate, Server_Players);
	DOREPLIFETIME(ALobbyGamestate, bShowMapChoice);

	DOREPLIFETIME(ALobbyGamestate, Farm);
	DOREPLIFETIME(ALobbyGamestate, WinterFarm);

	DOREPLIFETIME(ALobbyGamestate, MapChoiceLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, bMapChosen);
	
	DOREPLIFETIME(ALobbyGamestate, MaxPlayersOnServer);
}

void ALobbyGamestate::UpdateCharacterMaterial(int32 _Player,ECharacters _Character, ECharacterColours _CharacterColour)
{
	if (Server_Players.Num() > _Player)
	{
		if (auto playerState = Server_Players[_Player])
		{
			playerState->UpdateCharacterMaterial(_Character, _CharacterColour);
		}
	}
}

int32 ALobbyGamestate::GetNumberOfCharactersTaken(ECharacters _DesiredCharacter) const
{
	int characterCount{};
	if (Server_Players.Num() > 0)
	{
		for(auto playerState : Server_Players)
		{
			if (playerState.Get())
			{
				if (playerState->Character == _DesiredCharacter)
					characterCount++;
			}
		}
	}
	return characterCount;
}

int32 ALobbyGamestate::GetNumberOfCharacterColoursTaken(ECharacters _DesiredCharacter, ECharacterColours _DesiredCharacterColour) const
{
	int characterColourCount{};
	if (Server_Players.Num() > 0)
	{
		for(auto playerState : Server_Players)
		{
			if (playerState.Get())
			{
				if (playerState->Character == _DesiredCharacter && playerState->CharacterColour == _DesiredCharacterColour)
					characterColourCount++;
			}
		}
	}
	
	return characterColourCount;
}



