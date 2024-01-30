

#include "LobbyGamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Widgets/Widget_IngameMenu.h"
#include "Prototype2/Widgets/Widget_MapChoice.h"

ALobbyGamestate::ALobbyGamestate()
{
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = true;
}


void ALobbyGamestate::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (HasAuthority())
	{
		int RandomColour = rand() % ((int)EColours::MAXCOLOURS - 1);
		TeamOneColour = (EColours)RandomColour;
	
		do
		{
			RandomColour = rand() % ((int)EColours::MAXCOLOURS - 1);
		}
		while ((EColours)RandomColour == TeamOneColour);
		TeamTwoColour = (EColours)RandomColour;
	}
}

void ALobbyGamestate::BeginPlay()
{
	Super::BeginPlay();

	/* Pre set backup level */
	if (GameMode == 0) // Normal Mode
		MapChoice = "/Game/Maps/Level_FF_Large";
	else if (GameMode == 1) // Brawl Mode
		MapChoice = "/Game/Maps/Level_FF_Brawl";
	else if (GameMode == 2) // Blitz Mode
		MapChoice = "/Game/Maps/Level_FF_Blitz";
	else if (GameMode == 3) // Blitz Mode
		MapChoice = "/Game/Maps/Level_FF_Chaos";


}

void ALobbyGamestate::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	TickTimers(_DeltaSeconds);

	UpdateTeams();
}

void ALobbyGamestate::SetIsReady(int32 _Player, bool _bIsReady)
{
	if (Server_Players.Num() > _Player)
	{
		if (auto PlayerState = Server_Players[_Player])
		{
			PlayerState->SetIsReady(_bIsReady);
		}
	}

	bool isEveryoneReady{true};
	for(auto PlayerState : Server_Players)
	{
		if (!PlayerState->IsReady)
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
		Farm += 1;
	}
	else if (_Level == EFarm::WINTERFARM)
	{
		WinterFarm += 1;
	}
	else if (_Level == EFarm::HONEYFARM)
	{
		HoneyFarm += 1;
	}
	else if (_Level == EFarm::FLOATINGISLANDSFARM)
	{
		FloatingIslandFarm += 1;
	}
}

void ALobbyGamestate::UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails)
{
	for(auto LobbyPlayerState : Server_Players)
	{
		if(LobbyPlayerState->Player_ID == _Player)
		{
			LobbyPlayerState->Details = _CharacterDetails;
		}
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

int32 ALobbyGamestate::GetFloatingIslandFarm() const
{
	return FloatingIslandFarm;
}

bool ALobbyGamestate::HasMapBeenChosen() const
{
	return bMapChosen;
}

int32 ALobbyGamestate::GetMapChoiceTotalLengthSeconds() const
{
	return MapChoiceTotalLengthSeconds;
}

void ALobbyGamestate::SetGameMode(int32 _Mode)
{
	GameMode = _Mode;
}

int32 ALobbyGamestate::GetGameMode()
{
	return GameMode;
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
	DOREPLIFETIME(ALobbyGamestate, HoneyFarm);
	DOREPLIFETIME(ALobbyGamestate, FloatingIslandFarm);

	DOREPLIFETIME(ALobbyGamestate, MapChoiceTotalLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, MapChoiceLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, bMapChosen);
	DOREPLIFETIME(ALobbyGamestate, bHasCountedDown);
	
	DOREPLIFETIME(ALobbyGamestate, MaxPlayersOnServer);
	DOREPLIFETIME(ALobbyGamestate, GameMode);

	DOREPLIFETIME(ALobbyGamestate, Server_TeamOne);
	DOREPLIFETIME(ALobbyGamestate, Server_TeamTwo);
	DOREPLIFETIME(ALobbyGamestate, TeamOneColour);
	DOREPLIFETIME(ALobbyGamestate, TeamTwoColour);
	DOREPLIFETIME(ALobbyGamestate, bHasAllPlayersVoted);
}

void ALobbyGamestate::PickMapToPlay()
{
	//bShowMapChoice = true; // Show map choice

	//const int32 TotalVotes = Farm + WinterFarm + HoneyFarm + FloatingIslandFarm;
	//if (TotalVotes == Server_Players.Num() && bMapChosen == false)
	if (bMapChosen == false)
	{
		bMapChosen = true; // Turned true so that it will change HUD visibility for timer
		if (Farm > WinterFarm && Farm > HoneyFarm && Farm > FloatingIslandFarm) // Normal farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
				MapChoice = "/Game/Maps/Level_FF_Large";
			else if (GameMode == 1) // Brawl Mode
				MapChoice = "/Game/Maps/Level_FF_Brawl";
			else if (GameMode == 2) // Blitz Mode
				MapChoice = "/Game/Maps/Level_FF_Blitz";
			
		}
		else if (WinterFarm > Farm && WinterFarm > HoneyFarm && WinterFarm > FloatingIslandFarm) // Winter farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
				MapChoice = "/Game/Maps/Level_Winter_LargeV2";
			else if (GameMode == 1)// Brawl Mode
				MapChoice = "/Game/Maps/Level_Winter_Brawl";
			else if (GameMode == 2) // Blitz Mode
				MapChoice = "/Game/Maps/Level_Winter_Blitz";
		}
		else if (HoneyFarm > Farm && HoneyFarm > WinterFarm && HoneyFarm > FloatingIslandFarm) // Honey farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
				MapChoice = "/Game/Maps/Level_Honey_Large";
			else if (GameMode == 1) // Brawl Mode
				MapChoice = "/Game/Maps/Level_Honey_Brawl";
			else if (GameMode == 2) // Blitz Mode
				MapChoice = "/Game/Maps/Level_Honey_Blitz";
		}
		else if (FloatingIslandFarm > Farm && FloatingIslandFarm > WinterFarm && FloatingIslandFarm > HoneyFarm) // floating islands farm gets most votes
			{
			if (GameMode == 0) // Normal Mode
				MapChoice = "/Game/Maps/Level_SkyIsland";
			else
				UE_LOG(LogTemp, Warning, TEXT("Sky island brawl mode attempted to start")); // No brawl mode for floating islands
			}
		else // Pick a random map from highest votes
		{
			UE_LOG(LogTemp, Warning, TEXT("Selecting random map from top votes"));
			
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
								if (GameMode == 0) // Normal Mode
									MapChoice = "/Game/Maps/Level_FF_Large";
								else // Brawl Mode
									MapChoice = "/Game/Maps/Level_FF_Brawl";
								UE_LOG(LogTemp, Warning, TEXT("Friendly Farm Map Chosen"));
								break;
							}
						case 1:
							{
								if (GameMode == 0) // Normal Mode
									MapChoice = "/Game/Maps/Level_Winter_LargeV2";
								else // Brawl Mode
									MapChoice = "/Game/Maps/Level_Winter_Brawl";
								UE_LOG(LogTemp, Warning, TEXT("Winter Farm Map Chosen"));
								break;
							}
						case 2:
							{
								if (GameMode == 0) // Normal Mode
									MapChoice = "/Game/Maps/Level_Honey_Large";
								else // Brawl Mode
									MapChoice = "/Game/Maps/Level_Honey_Brawl";
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
		
		//if (MapChoiceTotalLengthSeconds > MapChoiceLengthSeconds)
		//{
		//	MapChoiceTotalLengthSeconds = MapChoiceLengthSeconds + 1.0f;
		//}
	}
}

void ALobbyGamestate::TickTimers(float _DeltaSeconds)
{
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
					if (GameMode != 3) // Only show map choice if not a chaos map
						bShowMapChoice = true; // Show map choice

					//PickMapToPlay();

					// Countdown between all players choosing map and actually starting
					
					if (MapChoiceTotalLengthSeconds > 0)
					{
						if (GameMode == 3) // Turn map choice value to 0 if on chaos (since no map choice)
							MapChoiceTotalLengthSeconds = 0; 
						
						MapChoiceTotalLengthSeconds -= _DeltaSeconds;

						const int32 TotalVotes = Farm + WinterFarm + HoneyFarm + FloatingIslandFarm;
						if (TotalVotes == Server_Players.Num() && bMapChosen == false && bHasAllPlayersVoted == false)
						{
							bHasAllPlayersVoted = true;
							if (MapChoiceTotalLengthSeconds > MapChoiceLengthSeconds)
							{
								MapChoiceTotalLengthSeconds = MapChoiceLengthSeconds; // + 1.0f;
							}
						}
						
						if (MapChoiceTotalLengthSeconds <= 0)
						{
							bIsCountingDown = false;

							/* Map selection unless chaos */
							if (GameMode == 3)
								MapChoice = "/Game/Maps/Level_FF_Chaos";
							else
								PickMapToPlay();
						
							if (auto GameInstance = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
							{
								GameInstance->FinalConnectionCount = Server_Players.Num();

								for(auto Player : Server_Players)
								{
									FString SomePlayerName{"UNASSIGNED"};
									if (auto NullSubsystem = IOnlineSubsystem::Get())
									{
										IOnlineIdentityPtr IdentityInterface = NullSubsystem->GetIdentityInterface();
										if (IdentityInterface.IsValid())
										{
											TSharedPtr<const FUniqueNetId> UserId = Player->GetUniqueId().GetUniqueNetId();
											if (UserId.IsValid())
											{
												SomePlayerName = UserId->ToString();
											}
										}
									}
									GameInstance->FinalPlayerDetails.Add(SomePlayerName, Player->Details);
								}
							}
							//UGameplayStatics::OpenLevel(GetWorld(), FName(MapChoice), true, "listen?bIsLanMatch=1");
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

void ALobbyGamestate::UpdateTeams()
{
	if (!bTeams)
		return;

	/* Team 1 */
	if (Server_TeamOne.Num() > 0)
	{
		for (int i = 0; i < Server_TeamOne.Num(); i++)
		{
			if (auto Player = Server_TeamOne[i])
			{
				if (Player->Details.Colour == TeamTwoColour)
				{
					Server_TeamTwo.Add(Player);
					Server_TeamOne.RemoveAt(i);
					i--;
				}
			}
		}
	}
	/* Team 2 */
	if (Server_TeamTwo.Num() > 0)
	{
		for (int i = 0; i < Server_TeamTwo.Num(); i++)
		{
			if (auto Player = Server_TeamTwo[i])
			{
				if (Player->Details.Colour == TeamOneColour)
				{
					Server_TeamOne.Add(Player);
					Server_TeamTwo.RemoveAt(i);
					i--;
				}
			}
		}
	}
}

void ALobbyGamestate::UpdateCharacterMaterial(int32 _Player,FCharacterDetails _Details)
{
	if (Server_Players.Num() > _Player)
	{
		if (auto playerState = Server_Players[_Player])
		{
			playerState->UpdateCharacterMaterial(_Details);
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
				if (playerState->Details.Character == _DesiredCharacter)
					characterCount++;
			}
		}
	}
	return characterCount;
}

int32 ALobbyGamestate::GetNumberOfCharacterColoursTaken(FCharacterDetails _Details) const
{
	int characterColourCount{};
	if (Server_Players.Num() > 0)
	{
		for(auto playerState : Server_Players)
		{
			if (playerState.Get())
			{
				if (playerState->Details.Character == _Details.Character && playerState->Details.CharacterColour == _Details.CharacterColour)
					characterColourCount++;
			}
		}
	}
	
	return characterColourCount;
}



