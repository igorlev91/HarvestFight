

#include "LobbyGamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Widgets/Widget_IngameMenu.h"
#include "Prototype2/Widgets/Widget_MapChoice.h"
#include "Prototype2/DataAssets/TeamNames.h"
#include "Prototype2/HUDS/LobbyHUD.h"
#include "Prototype2/Widgets/Widget_LobbyCharacterSelection.h"
#include "Prototype2/Widgets/Widget_LobbyPlayerHUDV2.h"

ALobbyGamestate::ALobbyGamestate()
{
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = true;

	static ConstructorHelpers::FObjectFinder<UTeamNames> FoundTeamNamesData(TEXT("/Game/DataAssets/MiscData/DA_TeamNames"));
	if (IsValid(FoundTeamNamesData.Object))
		TeamNamesData = FoundTeamNamesData.Object;
}


void ALobbyGamestate::OnRep_TeamsDetails()
{
	/*if (!bTeams)
		return;
	
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (IsValid(MyCharacter))
	{
		ALobbyCharacter* MyCastedCharacter = Cast<ALobbyCharacter>(MyCharacter);
		if (IsValid(MyCastedCharacter))
		{
			if (IsValid(MyCastedCharacter->HUD))
			{
				if (TeamsDetails.Server_TeamOne.Contains(MyCastedCharacter->GetPlayerState<ALobbyPlayerState>()))
				{
					MyCastedCharacter->HUD->HUDWidget->WBP_LobbyCharacterSelection->OnRep_TeamsDetails(true);
				}
				else
				{
					MyCastedCharacter->HUD->HUDWidget->WBP_LobbyCharacterSelection->OnRep_TeamsDetails(false);
				}
			}
		}
	}*/
}

void ALobbyGamestate::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ALobbyGamestate::BeginPlay()
{
	Super::BeginPlay();
	
	/* Pre set backup level */
	if (GameMode == 0) // Normal Mode
		MapChoice = FriendlyFarmClassicLarge;
	else if (GameMode == 1) // Brawl Mode
		MapChoice = FriendlyFarmBrawl;
	else if (GameMode == 2) // Blitz Mode
		MapChoice = FriendlyFarmBlitz;
	//else if (GameMode == 3) // Chaos Mode
	//	MapChoice = Chaos;
}

void ALobbyGamestate::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	TickTimers(_DeltaSeconds);

	UpdateTeams();

	ServerTravel(_DeltaSeconds);
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
	else if (_Level == EFarm::CLOCKWORKFARM)
	{
		ClockworkFarm += 1;
	}
	else if (_Level == EFarm::RANDOMFARM)
	{
		RandomFarm += 1;
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

int32 ALobbyGamestate::GetClockworkFarm() const
{
	return ClockworkFarm;
}

int32 ALobbyGamestate::GetRandomFarm() const
{
	return RandomFarm;
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
	DOREPLIFETIME(ALobbyGamestate, HoneyFarm);
	DOREPLIFETIME(ALobbyGamestate, FloatingIslandFarm);
	DOREPLIFETIME(ALobbyGamestate, ClockworkFarm);
	DOREPLIFETIME(ALobbyGamestate, RandomFarm);

	DOREPLIFETIME(ALobbyGamestate, MapChoiceTotalLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, MapChoiceLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, bMapChosen);
	DOREPLIFETIME(ALobbyGamestate, bHasCountedDown);
	DOREPLIFETIME(ALobbyGamestate, MapChoice);
	
	DOREPLIFETIME(ALobbyGamestate, MaxPlayersOnServer);
	
	DOREPLIFETIME(ALobbyGamestate, TeamsDetails);
	DOREPLIFETIME(ALobbyGamestate, bHasAllPlayersVoted);

	DOREPLIFETIME(ALobbyGamestate, bCanTravel);
	
}

void ALobbyGamestate::PickMapToPlay()
{
	//bShowMapChoice = true; // Show map choice

	//const int32 TotalVotes = Farm + WinterFarm + HoneyFarm + FloatingIslandFarm;
	//if (TotalVotes == Server_Players.Num() && bMapChosen == false)
	if (bMapChosen == false)
	{
		bMapChosen = true; // Turned true so that it will change HUD visibility for timer
		if (Farm > WinterFarm && Farm > HoneyFarm && Farm > FloatingIslandFarm && Farm > ClockworkFarm && Farm >= RandomFarm) // Normal farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
			{
				if (Server_Players.Num() <= 2)	
					MapChoice = FriendlyFarmClassicSmall;
				else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
					MapChoice = FriendlyFarmClassicMedium;
				else
					MapChoice = FriendlyFarmClassicLarge;
			}
			else if (GameMode == 1) // Brawl Mode
				MapChoice = FriendlyFarmBrawl;
			else if (GameMode == 2) // Blitz Mode
				MapChoice = FriendlyFarmBlitz;
			else
				UE_LOG(LogTemp, Warning, TEXT("Attempted to start friendly farm map but incorrect mode attached"));
			
		}
		else if (WinterFarm > Farm && WinterFarm > HoneyFarm && WinterFarm > FloatingIslandFarm && WinterFarm > ClockworkFarm && WinterFarm >= RandomFarm) // Winter farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
			{
				if (Server_Players.Num() <= 2)	
					MapChoice = FrostyFieldsClassicSmall;
				else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
					MapChoice = FrostyFieldsClassicMedium;
				else
					MapChoice = FrostyFieldsClassicLarge;
			}
			else if (GameMode == 1)// Brawl Mode
				MapChoice = FrostyFieldsBrawl;
			else if (GameMode == 2) // Blitz Mode
				MapChoice = FrostyFieldsBlitz;
			else
				UE_LOG(LogTemp, Warning, TEXT("Attempted to start winter map but incorrect mode attached"));
		}
		else if (HoneyFarm > Farm && HoneyFarm > WinterFarm && HoneyFarm > FloatingIslandFarm && HoneyFarm > ClockworkFarm && HoneyFarm >= RandomFarm) // Honey farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
			{
				if (Server_Players.Num() <= 2)	
					MapChoice = HoneyClassicSmall;
				else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
					MapChoice = HoneyClassicMedium;
				else
					MapChoice = HoneyClassicLarge;
			}
			else if (GameMode == 1) // Brawl Mode
				MapChoice = HoneyBrawl;
			else
				UE_LOG(LogTemp, Warning, TEXT("Attempted to start honey map but incorrect mode attached"));
		}
		else if (FloatingIslandFarm > Farm && FloatingIslandFarm > WinterFarm && FloatingIslandFarm > HoneyFarm && FloatingIslandFarm > ClockworkFarm && FloatingIslandFarm >= RandomFarm) // floating islands farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
				MapChoice = FloatingIslandsClassic;
			else if (GameMode == 1) // Brawl Mode
				MapChoice = FloatingIslandsBrawl;
			else if (GameMode == 2) // Blitz Mode
				MapChoice = FloatingIslandsBlitz;
			else
				UE_LOG(LogTemp, Warning, TEXT("Attempted to start floating farm map but incorrect mode attached"));
		}
		else if (ClockworkFarm > Farm && ClockworkFarm > WinterFarm && ClockworkFarm > HoneyFarm && ClockworkFarm > FloatingIslandFarm && ClockworkFarm >= RandomFarm) // clockwork farm gets most votes
		{
			if (GameMode == 0) // Normal Mode
			{
				if (Server_Players.Num() <= 4)	
					MapChoice = ClockworkClassicMedium;
				else
					MapChoice = ClockworkClassicLarge;
			}
			else if (GameMode == 1) // Brawl Mode
			{
				UE_LOG(LogTemp, Warning, TEXT("ClockworkFarm brawl mode attempted to start")); // No brawl mode for ClockworkFarm
			}
			else
				MapChoice = ClockworkBlitz;
		}
		else if (RandomFarm > Farm && RandomFarm > WinterFarm && RandomFarm > HoneyFarm && RandomFarm > FloatingIslandFarm && RandomFarm > ClockworkFarm) // floating islands farm gets most votes
		{
			PickRandomMap();
		}
		else // Pick a random map from highest votes
		{
			UE_LOG(LogTemp, Warning, TEXT("Selecting random map from top votes"));
			
			TArray<int32> MapChoiceArray{Farm, WinterFarm, HoneyFarm, FloatingIslandFarm, ClockworkFarm};
			TArray<int32> MapChoiceTopVotesArray{false, false, false, false, false};
			//UE_LOG(LogTemp, Warning, TEXT("MapChoiceArray size: %d"), MapChoiceArray.Num());
			//UE_LOG(LogTemp, Warning, TEXT("MapChoiceTopVotesArray size: %d"), MapChoiceTopVotesArray.Num());

			//UE_LOG(LogTemp, Warning, TEXT("Gamemode: %d"), GameMode);
			
			/* Remove redundant maps from brawl/blitz choice */
			if (GameMode == 1) // Brawl
			{
				MapChoiceArray.RemoveAt(4); // Remove clockwork
				MapChoiceTopVotesArray.RemoveAt(4); // Remove 1 element
				//NumberOfMaps = 4;
				//UE_LOG(LogTemp, Warning, TEXT("Removing elements based on Brawl mode"));
				//UE_LOG(LogTemp, Warning, TEXT("MapChoiceArray size: %d"), MapChoiceArray.Num());
				//UE_LOG(LogTemp, Warning, TEXT("MapChoiceTopVotesArray size: %d"), MapChoiceArray.Num());
			}
			else if (GameMode == 2) // Blitz
			{
				MapChoiceArray.RemoveAt(4); // Remove honey farm
				MapChoiceArray.RemoveAt(2); // Remove clockwork
				MapChoiceTopVotesArray.RemoveAt(4); // Remove 1 element
				MapChoiceTopVotesArray.RemoveAt(3); // Remove 1 element
				//NumberOfMaps = 3;
				//UE_LOG(LogTemp, Warning, TEXT("Removing elements based on Blitz mode"));
				//UE_LOG(LogTemp, Warning, TEXT("MapChoiceArray size: %d"), MapChoiceArray.Num());
			}
			
			int32 HighestVote = 0;
			/* Work out the highest vote */
			for (int32 i = 0; i < MapChoiceArray.Num(); i++)
			{
				if (MapChoiceArray[i] >= HighestVote)
					HighestVote = MapChoiceArray[i];
			}
			/* Set which map choices got top votes */
			int32 NumberOfHighVotes = 0;
			for (int32 i = 0; i < MapChoiceArray.Num(); i++)
			{
				if (MapChoiceArray[i] == HighestVote)
				{
					MapChoiceTopVotesArray[i] = true;
					NumberOfHighVotes += 1;
				}
				//UE_LOG(LogTemp, Warning, TEXT("MapChoiceTopVotesArray: %s"), ( MapChoiceTopVotesArray[i] ? TEXT("true") : TEXT("false") ));
			}
			UE_LOG(LogTemp, Warning, TEXT("Number of votes: %d"), NumberOfHighVotes);
			/* Picking random map from highest votes */
			int32 RandomNumber = FMath::RandRange(0, NumberOfHighVotes - 1);
			UE_LOG(LogTemp, Warning, TEXT("Random Number: %d"), RandomNumber);
			/* Working out which map was chosen by random number */
			for (int32 i = 0; i < MapChoiceArray.Num(); i++)
			{
				if (MapChoiceTopVotesArray[i] == false)
				{
					RandomNumber++;
					UE_LOG(LogTemp, Warning, TEXT("Random Number Now: %d"), RandomNumber);
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
								if (GameMode == 0) // Classic Mode friendly farm
								{
									if (Server_Players.Num() <= 2)	
										MapChoice = FriendlyFarmClassicSmall;
									else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
										MapChoice = FriendlyFarmClassicMedium;
									else
										MapChoice = FriendlyFarmClassicLarge;
								}
								else if (GameMode == 1) // Brawl Mode friendly farm
									MapChoice = FriendlyFarmBrawl;
								else if (GameMode == 2) // Brawl Mode friendly farm
									MapChoice = FriendlyFarmBlitz;
								else
									UE_LOG(LogTemp, Warning, TEXT("Error: Attempted to access incorrect split vote map - case 0"));
								
								UE_LOG(LogTemp, Warning, TEXT("Friendly Farm Map Chosen"));
								break;
							}
						case 1:
							{
								if (GameMode == 0) // Frosty Fields Classic Mode
								{
									if (Server_Players.Num() <= 2)	
										MapChoice = FrostyFieldsClassicSmall;
									else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
										MapChoice = FrostyFieldsClassicMedium;
									else
										MapChoice = FrostyFieldsClassicLarge;
								}
								else if (GameMode == 1) // Frosty Fields Brawl Mode
									MapChoice = FrostyFieldsBrawl;
								else if (GameMode == 2) // Frosty Fields Blitz Mode
									MapChoice = FrostyFieldsBlitz;
								else
									UE_LOG(LogTemp, Warning, TEXT("Error: Attempted to access incorrect split vote map - case 1"));
								
								UE_LOG(LogTemp, Warning, TEXT("Winter Farm Map Chosen"));
								break;
							}
						case 2:
							{
								if (GameMode == 0) // Honey Classic Mode
								{
									if (Server_Players.Num() <= 2)	
										MapChoice = HoneyClassicSmall;
									else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
										MapChoice = HoneyClassicMedium;
									else
										MapChoice = HoneyClassicLarge;
									UE_LOG(LogTemp, Warning, TEXT("Honey Farm Map Chosen"));
								}
								else if (GameMode == 1) // Honey Brawl Mode
								{
									MapChoice = HoneyBrawl;
									UE_LOG(LogTemp, Warning, TEXT("Honey Farm Map Chosen"));
								}
								else if (GameMode == 2) // Floating island Blitz Mode
								{
									MapChoice = FloatingIslandsBlitz;
									UE_LOG(LogTemp, Warning, TEXT("Floating Island Farm Map Chosen"));
								}
								else
									UE_LOG(LogTemp, Warning, TEXT("Error: Attempted to access incorrect split vote map - case 2"));

								break;
							}
						case 3:
							{
								if (GameMode == 0) // Classic Mode floating island
									MapChoice = FloatingIslandsClassic;
								else if (GameMode == 1) // Brawl Mode floating island
									MapChoice = FloatingIslandsBrawl;
								else
									UE_LOG(LogTemp, Warning, TEXT("Error: Attempted to access incorrect split vote map - case 3"));
								
								UE_LOG(LogTemp, Warning, TEXT("Floating Island Map Chosen"));
								break;
							}
						case 4:
							{
								/* Clockwork map choice */
								if (GameMode == 0) // Normal Mode
								{
									if (Server_Players.Num() <= 4)	
										MapChoice = ClockworkClassicMedium;
									else
										MapChoice = ClockworkClassicLarge;
								}
								else if (GameMode == 1) // Brawl Mode
								{
									UE_LOG(LogTemp, Warning, TEXT("ClockworkFarm brawl mode attempted to start")); // No brawl mode for ClockworkFarm
								}
								else
								{
									MapChoice = ClockworkBlitz;
								}
								
								UE_LOG(LogTemp, Warning, TEXT("Clockwork Map Chosen"));
								break;
							}
						default:
							{
								break;
							}
						}
						break;
					}
				}
			}
		}
	}
}

void ALobbyGamestate::PickRandomMap()
{
	UE_LOG(LogTemp, Warning, TEXT("Random Map button received highest vote. Randomising level based on game mode"));
	
	switch (GameMode)
	{
	case 0: // Classic
		{
			int32 RandomNumber = FMath::RandRange(0, 4);
			UE_LOG(LogTemp, Warning, TEXT("Random Number: %d"), RandomNumber);

			switch (RandomNumber)
			{
			case 0:
				{
					if (Server_Players.Num() <= 2)	
						MapChoice = FriendlyFarmClassicSmall;
					else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
						MapChoice = FriendlyFarmClassicMedium;
					else
						MapChoice = FriendlyFarmClassicLarge;

					UE_LOG(LogTemp, Warning, TEXT("Friendly Farm Map Chosen"));
					break;
				}
			case 1:
				{
					if (Server_Players.Num() <= 2)	
						MapChoice = FrostyFieldsClassicSmall;
					else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
						MapChoice = FrostyFieldsClassicMedium;
					else
						MapChoice = FrostyFieldsClassicLarge;

					UE_LOG(LogTemp, Warning, TEXT("Winter Farm Map Chosen"));
					break;
				}
			case 2:
				{
					if (Server_Players.Num() <= 2)	
						MapChoice = HoneyClassicSmall;
					else if (Server_Players.Num() == 3 || Server_Players.Num() == 4)	
						MapChoice = HoneyClassicMedium;
					else
						MapChoice = HoneyClassicLarge;

					UE_LOG(LogTemp, Warning, TEXT("Honey Farm Map Chosen"));
					break;
				}
			case 3:
				{
					MapChoice = FloatingIslandsClassic;

					UE_LOG(LogTemp, Warning, TEXT("Floating Island Farm Map Chosen"));
					break;
				}
			case 4:
				{
					if (Server_Players.Num() <= 4)	
						MapChoice = ClockworkClassicMedium;
					else
						MapChoice = ClockworkClassicLarge;

					UE_LOG(LogTemp, Warning, TEXT("Clockwork Farm Map Chosen"));
					break;
				}
			default:
				{
					MapChoice = FriendlyFarmClassicLarge;

					UE_LOG(LogTemp, Warning, TEXT("Default case reached. Large classic friendly farm chosen"));
					break;
				}
			}
			
			break;
		}
	case 1: // Brawl
		{
			int32 RandomNumber = FMath::RandRange(0, 3);
			UE_LOG(LogTemp, Warning, TEXT("Random Number: %d"), RandomNumber);

			switch (RandomNumber)
			{
			case 0:
				{
					MapChoice = FriendlyFarmBrawl;

					UE_LOG(LogTemp, Warning, TEXT("Friendly farm brawl map chosen"));
					break;
				}
			case 1:
				{
					MapChoice = FrostyFieldsBrawl;

					UE_LOG(LogTemp, Warning, TEXT("Winter farm brawl map chosen"));
					break;
				}
			case 2:
				{
					MapChoice = HoneyBrawl;

					UE_LOG(LogTemp, Warning, TEXT("Honey farm brawl map chosen"));
					break;
				}
			case 3:
				{
					MapChoice = FloatingIslandsBrawl;

					UE_LOG(LogTemp, Warning, TEXT("Floating island farm brawl map chosen"));
					break;
				}
			default:
				{
					MapChoice = FriendlyFarmBrawl;

					UE_LOG(LogTemp, Warning, TEXT("Default case reached. Friendly farm brawl map selected"));
					break;
				}
			}
			break;
		}
	case 2: // Blitz
		{
			int32 RandomNumber = FMath::RandRange(0, 3);
			UE_LOG(LogTemp, Warning, TEXT("Random Number: %d"), RandomNumber);

			switch (RandomNumber)
			{
			case 0:
				{
					MapChoice = FriendlyFarmBlitz;

					UE_LOG(LogTemp, Warning, TEXT("Friendly farm blitz map chosen"));
					break;
				}
			case 1:
				{
					MapChoice = FrostyFieldsBlitz;

					UE_LOG(LogTemp, Warning, TEXT("Winter island farm blitz map chosen"));
					break;
				}
			case 2:
				{
					MapChoice = FloatingIslandsBlitz;

					UE_LOG(LogTemp, Warning, TEXT("Floating island farm blitz map chosen"));
					break;
				}
			case 3:
				{
					MapChoice = ClockworkBlitz;

					UE_LOG(LogTemp, Warning, TEXT("Clockwork farm blitz map chosen"));
					break;
				}
			default:
				{
					MapChoice = FriendlyFarmBlitz;

					UE_LOG(LogTemp, Warning, TEXT("Default case reached, Friendly farm blitz map chosen"));
					break;
				}
			}
			break;
		}
	default:
		{
			UE_LOG(LogTemp, Warning, TEXT("Default case reached, incorrect gamemode reached"));
			break;
		}
	}
}

void ALobbyGamestate::TickTimers(float _DeltaSeconds)
{
	if (HasAuthority())
	{
		auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
		
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
					//if (GameMode != 3) // Only show map choice if not a chaos map
						bShowMapChoice = true; // Show map choice

					//PickMapToPlay();

					// Countdown between all players choosing map and actually starting
					
					if (MapChoiceTotalLengthSeconds > 0)
					{
						if (GameMode == 3) // Turn map choice value to 0 if on chaos (since no map choice)
							MapChoiceTotalLengthSeconds = 0; 
						
						MapChoiceTotalLengthSeconds -= _DeltaSeconds;

						const int32 TotalVotes = Farm + WinterFarm + HoneyFarm + FloatingIslandFarm + ClockworkFarm + RandomFarm;
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
							
							GameInstance->FinalPlayerDetails.Empty();
							for(auto Player : Server_Players)
							{
								GameInstance->FinalPlayerDetails.Add({Player->GetPlayerName(), Player->Details});
								UE_LOG(LogTemp, Warning, TEXT("%s Added To FinalDetails With Color: %s"), *Player->GetPlayerName(), *FString::FromInt((int16)Player->Details.Colour));
							}

							GameInstance->FinalConnectionCount = Server_Players.Num();
							GameInstance->bTeams = bTeams;
							GameInstance->TeamOneName = TeamsDetails.TeamOneName;
							GameInstance->TeamTwoName = TeamsDetails.TeamTwoName;
							GameInstance->FinalTeamACount = TeamsDetails.Server_TeamOne.Num();
							GameInstance->FinalTeamBCount = TeamsDetails.Server_TeamTwo.Num();
							GameInstance->FinalTeamAColour = TeamsDetails.TeamOneColour;
							GameInstance->FinalTeamBColour = TeamsDetails.TeamTwoColour;

							//GetWorld()->ServerTravel(MapChoice, false, false); // Start level
							bCanTravel = true;
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
	if (TeamsDetails.Server_TeamOne.Num() > 0)
	{
		for (int i = 0; i < TeamsDetails.Server_TeamOne.Num(); i++)
		{
			if (auto Player = TeamsDetails.Server_TeamOne[i])
			{
				if (Player->Details.Colour == TeamsDetails.TeamTwoColour)
				{
					TeamsDetails.Server_TeamTwo.Add(Player);
					TeamsDetails.Server_TeamOne.RemoveAt(i);
					i--;
				}
			}
		}
	}
	/* Team 2 */
	if (TeamsDetails.Server_TeamTwo.Num() > 0)
	{
		for (int i = 0; i < TeamsDetails.Server_TeamTwo.Num(); i++)
		{
			if (auto Player = TeamsDetails.Server_TeamTwo[i])
			{
				if (Player->Details.Colour == TeamsDetails.TeamOneColour)
				{
					TeamsDetails.Server_TeamOne.Add(Player);
					TeamsDetails.Server_TeamTwo.RemoveAt(i);
					i--;
				}
			}
		}
	}
}

void ALobbyGamestate::ServerTravel(float _DeltaSeconds)
{
	if (!HasAuthority())
		return;
	
	if (bCanTravel == false)
		return;

	MapTravelTimer -= _DeltaSeconds;

	if (MapTravelTimer <= 0)
	{
		bCanTravel = false;
		GetWorld()->ServerTravel(MapChoice, false, false); // Start level
	}
}

void ALobbyGamestate::OnRep_CanTravel()
{
	auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
	GameInstance->FinalConnectionCount = Server_Players.Num();
	GameInstance->bTeams = bTeams;
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

void ALobbyGamestate::CheckAllReady()
{
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



