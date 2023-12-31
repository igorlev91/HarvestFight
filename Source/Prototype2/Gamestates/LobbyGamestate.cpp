

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
					PickRandomMapToPlay();

					// Countdown between all players choosing map and actually starting
					MapChoiceTotalLengthSeconds -= _DeltaSeconds;
					if (MapChoiceTotalLengthSeconds <= 0)
					{
						bIsCountingDown = false;
						
						if (auto GameInstance = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
						{
							GameInstance->FinalConnectionCount = Server_Players.Num();

							for(auto Player : Server_Players)
							{
								FCharacterDetails Details{};
								FString PlayerName{};
								IOnlineIdentityPtr IdentityInterface = IOnlineSubsystem::Get()->GetIdentityInterface();
								if (IdentityInterface.IsValid())
								{
									PlayerName = IdentityInterface->GetPlayerNickname(*Player->GetUniqueId().GetUniqueNetId());
								}
								Details.Character = Player->Character;
								Details.CharacterColour = Player->CharacterColour;
								
								//UE_LOG(LogTemp, Warning, TEXT("%s Has Character %s and Colour %s"), *PlayerName, *FString::FromInt((int)Details.Character),*FString::FromInt((int)Details.CharacterColour) );
								GameInstance->FinalPlayerDetails.Add(PlayerName, Details);
							}
						}
						GetWorld()->ServerTravel(MapChoice, false, false); // Start level
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
	else if (_Level == EFarm::HONEYFARM)
	{
		HoneyFarm = HoneyFarm + 1;
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

int32 ALobbyGamestate::GetHoneyFarm() const
{
	return HoneyFarm;
}

bool ALobbyGamestate::HasMapBeenChosen() const
{
	return bMapChosen;
}

int32 ALobbyGamestate::GetMapChoiceTotalLengthSeconds() const
{
	return MapChoiceTotalLengthSeconds;
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

	DOREPLIFETIME(ALobbyGamestate, MapChoiceTotalLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, bMapChosen);
	
	DOREPLIFETIME(ALobbyGamestate, MaxPlayersOnServer);
}

void ALobbyGamestate::PickRandomMapToPlay()
{
	bShowMapChoice = true; // Show map choice

	const int32 TotalVotes = Farm + WinterFarm + HoneyFarm;
	if (TotalVotes == Server_Players.Num() && bMapChosen == false)
	{
		bMapChosen = true; // Turned true so that it will change HUD visibility for timer
		if (Farm > WinterFarm && Farm > HoneyFarm) // Normal farm gets most votes
		{
			MapChoice = "Level_Main";
		}
		else if (WinterFarm > Farm && WinterFarm > HoneyFarm) // Winter farm gets most votes
		{
			MapChoice = "Level_Winter";
		}
		else if (HoneyFarm > Farm && HoneyFarm > WinterFarm) // Honey farm gets most votes
		{
			MapChoice = "Level_Honey";
		}
		else // Pick a random map from highest votes
		{
			int32 MapChoiceArray[3] = {Farm, WinterFarm, HoneyFarm};
			bool MapChoiceTopVotesArray[3] = {false, false, false};
			int32 HighestVote = 0;
			/* Work out the highest vote */
			for (int32 i = 0; i < NumberOfMaps; i++)
			{
				if (MapChoiceArray[i] >= HighestVote)
					HighestVote = MapChoiceArray[i];
			}
			/* Set which map choices got top votes */
			int32 NumberOfHighVotes = 0;
			for (int32 i = 0; i < NumberOfMaps; i++)
			{
				if (MapChoiceArray[i] == HighestVote)
				{
					MapChoiceTopVotesArray[i] = true;
					UE_LOG(LogTemp, Warning, TEXT("True"));
					NumberOfHighVotes += 1;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Number of votes: %d"), NumberOfHighVotes);
			/* Picking random map from highest votes */
			int32 RandomNumber = FMath::RandRange(0, NumberOfHighVotes - 1);
			UE_LOG(LogTemp, Warning, TEXT("Random Number: %d"), RandomNumber);
			/* Working out which map was chosen by random number */
			for (int32 i = 0; i < NumberOfMaps; i++)
			{
				if (MapChoiceTopVotesArray[i] == false)
				{
					RandomNumber++;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Array elements checked: %d"), i);
					if (i == RandomNumber)
					{
						switch (i)
						{
						case 0:
							{
								MapChoice = "Level_Main";
								UE_LOG(LogTemp, Warning, TEXT("Friendly Farm Map Chosen"));
								break;
							}
						case 1:
							{
								MapChoice = "Level_Winter";
								UE_LOG(LogTemp, Warning, TEXT("Winter Farm Map Chosen"));
								break;
							}
						case 2:
							{
								MapChoice = "Level_Honey";
								UE_LOG(LogTemp, Warning, TEXT("Honey Farm Map Chosen"));
								break;
							}
						default:
							{
								break;
							}
						}
						i = NumberOfMaps;
					}
				}
			}
		}
		if (MapChoiceTotalLengthSeconds > MapChoiceLengthSeconds)
		{
			MapChoiceTotalLengthSeconds = MapChoiceLengthSeconds + 1.0f;
		}
	}
}

void ALobbyGamestate::UpdateCharacterMaterial(int32 _Player,ECharacters _Character, FVector4d _CharacterColour)
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

int32 ALobbyGamestate::GetNumberOfCharacterColoursTaken(ECharacters _DesiredCharacter, FVector _DesiredCharacterColour) const
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



