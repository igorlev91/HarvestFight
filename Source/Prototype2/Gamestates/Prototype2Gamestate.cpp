

#include "Prototype2Gamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/DataAssets/DataAssetWorldOverride.h"
#include "Prototype2/DataAssets/WorldOverrideData.h"

APrototype2Gamestate::APrototype2Gamestate()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UWorldOverrideData> FoundDefaultWorldData(TEXT("/Game/DataAssets/WorldOverride/DA_DefaultWorld_Data"));
	if (IsValid(FoundDefaultWorldData.Object))
		DefaultWorldOverrideData = FoundDefaultWorldData.Object;
}

void APrototype2Gamestate::BeginPlay()
{
	Super::BeginPlay();

	if (!GameModeRef)
	{
		AGameModeBase* ThisGameMode = UGameplayStatics::GetGameMode(GetWorld());
		GameModeRef = Cast<APrototype2GameMode>(ThisGameMode);
	}

	if (!GameInstanceRef)
	{
		UPrototypeGameInstance* GameInstance = Cast<UPrototypeGameInstance>(GetGameInstance());
		GameInstanceRef = GameInstance;
	}
		
}

void APrototype2Gamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TickCountdownTimer(DeltaSeconds);
	TickMatchTimer(DeltaSeconds);
	TickEndGameTimer(DeltaSeconds);

	//UpdateTeamsScores();

	TickMapRotationTimer(DeltaSeconds);

	TickTimers(DeltaSeconds);

	ServerTravel(DeltaSeconds);
}

void APrototype2Gamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, bIsLastMinute);
	DOREPLIFETIME(APrototype2Gamestate, CountdownLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, CountdownLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, bGameHasStarted);
	DOREPLIFETIME(APrototype2Gamestate, bHasGameFinished);
	DOREPLIFETIME(APrototype2Gamestate, bGameReadyForVote);

	DOREPLIFETIME(APrototype2Gamestate, EndGameLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, EndGameLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, BriefTimesUpEndGameLengthSeconds);
	
	DOREPLIFETIME(APrototype2Gamestate, bIsCountingDown);
	DOREPLIFETIME(APrototype2Gamestate, bPreviousServerTravel);
	
	DOREPLIFETIME(APrototype2Gamestate, Server_Players);
	
	DOREPLIFETIME(APrototype2Gamestate, MaxPlayersOnServer);
	DOREPLIFETIME(APrototype2Gamestate, FinalConnectionCount);

	DOREPLIFETIME(APrototype2Gamestate, PlayerWinner);
	DOREPLIFETIME(APrototype2Gamestate, WinningScore);

	DOREPLIFETIME(APrototype2Gamestate, SellMultiplier);

	DOREPLIFETIME(APrototype2Gamestate, TeamOneColour);
	DOREPLIFETIME(APrototype2Gamestate, TeamTwoColour);
	DOREPLIFETIME(APrototype2Gamestate, TeamOneName);
	DOREPLIFETIME(APrototype2Gamestate, TeamTwoName);
	DOREPLIFETIME(APrototype2Gamestate, Server_TeamOne);
	DOREPLIFETIME(APrototype2Gamestate, Server_TeamTwo);

	DOREPLIFETIME(APrototype2Gamestate, bTeams);

	DOREPLIFETIME(APrototype2Gamestate, Team1Points);
	DOREPLIFETIME(APrototype2Gamestate, Team2Points);
	DOREPLIFETIME(APrototype2Gamestate, ExtraSettings);

	DOREPLIFETIME(APrototype2Gamestate, FinalTeamACount);
	DOREPLIFETIME(APrototype2Gamestate, FinalTeamBCount);
	
	DOREPLIFETIME(APrototype2Gamestate, MapRotationShowTimer);
	DOREPLIFETIME(APrototype2Gamestate, bIsMapChoiceShowing);

	DOREPLIFETIME(APrototype2Gamestate, Farm);
	DOREPLIFETIME(APrototype2Gamestate, WinterFarm);
	DOREPLIFETIME(APrototype2Gamestate, HoneyFarm);
	DOREPLIFETIME(APrototype2Gamestate, FloatingIslandFarm);
	DOREPLIFETIME(APrototype2Gamestate, ClockworkFarm);
	DOREPLIFETIME(APrototype2Gamestate, RandomFarm);

	DOREPLIFETIME(APrototype2Gamestate, MapChoiceTotalLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, MapChoiceLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, bMapChosen);
	DOREPLIFETIME(APrototype2Gamestate, bHasAllPlayersVoted);
	DOREPLIFETIME(APrototype2Gamestate, MapChoice);
	DOREPLIFETIME(APrototype2Gamestate, bCanTravel);

	DOREPLIFETIME(APrototype2Gamestate, TheCrown);
}

void APrototype2Gamestate::TickCountdownTimer(float DeltaSeconds)
{
	if (HasAuthority() && !bGameHasStarted)
	{
		if (Server_Players.Num() >= FinalConnectionCount)
		{
			if (CountdownLengthSeconds > 0)
			{
				CountdownLengthSeconds -= DeltaSeconds;
			}
			else
			{
				CountdownLengthMinutes--;
				CountdownLengthSeconds = 60;
			}
			
			if (CountdownLengthSeconds <= 0 && CountdownLengthMinutes <= 0)
			{
				bGameHasStarted = true;
				UE_LOG(LogTemp, Warning, TEXT("Countdown completed"));
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( GameHasStarted ? TEXT("true") : TEXT("false") ));
	}
}

void APrototype2Gamestate::TickMatchTimer(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( GameHasStarted ? TEXT("true") : TEXT("false") ));
	if (HasAuthority() && bGameHasStarted && !bHasGameFinished)
	{
		if (MatchLengthSeconds > 0)
		{
			MatchLengthSeconds -= DeltaSeconds;

			if (MatchLengthMinutes <= 0 && MatchLengthSeconds <= 0)
			{
				bHasGameFinished = true;
				PupeteerCharactersForEndGame();
			}

			/* Set final minute settings */
			// MOVED TO URandomEventManager
			//if (MatchLengthMinutes <= 1 && MatchLengthSeconds <= 0 && bIsLastMinute == false)
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("Final minute of gameplay"));
			//	bIsLastMinute = true;
			//	SellMultiplier = 2;
			//}
		}
		else
		{
			MatchLengthMinutes--;
			MatchLengthSeconds = 60;
		}
	}
}

void APrototype2Gamestate::TickEndGameTimer(float DeltaSeconds)
{
	// check if game has "finished"
	if (bHasGameFinished && !bGameReadyForVote)
	{
		if (EndGameLengthSeconds > 0)
		{
			EndGameLengthSeconds -= DeltaSeconds;
		}
		else
		{
			EndGameLengthMinutes--;
			EndGameLengthSeconds = 60;
		}

		if (EndGameLengthSeconds <= 0.0f && EndGameLengthMinutes <= 0)
		{
			bGameReadyForVote = true;
		}
	}
}

int32 APrototype2Gamestate::GetFinalConnectionCount() const
{
	return FinalConnectionCount;
}

void APrototype2Gamestate::SetFinalConnectionCount(int32 _FinalConnectionCount)
{
	FinalConnectionCount = _FinalConnectionCount;
}

void APrototype2Gamestate::SetMaxPlayersOnServer(int32 _FinalConnectionCount)
{
	MaxPlayersOnServer = _FinalConnectionCount;
}

void APrototype2Gamestate::SetPlayerWinner(int32 _WinnerID)
{
	PlayerWinner = _WinnerID;
}

void APrototype2Gamestate::SetWinningScore(int32 _WinningScore)
{
	WinningScore = _WinningScore;
}

int32 APrototype2Gamestate::RegisterPlayer(APrototype2PlayerState* _Player)
{
	return Server_Players.Add(_Player);
}

void APrototype2Gamestate::UnRegisterPlayer(APrototype2PlayerState* _Player)
{
	Server_Players.Remove(_Player);
}

int32 APrototype2Gamestate::GetCurrentConnectionCount()
{
	return Server_Players.Num();
}

bool APrototype2Gamestate::HasGameFinished()
{
	return bHasGameFinished;
}

bool APrototype2Gamestate::HasGameStarted()
{
	return bGameHasStarted;
}

bool APrototype2Gamestate::IsGameReadyForVote()
{
	return bGameReadyForVote;
}

int32 APrototype2Gamestate::GetMatchLengthMinutes()
{
	return MatchLengthMinutes;
}

void APrototype2Gamestate::SetMatchLengthMinutes(int32 _Minutes)
{
	MatchLengthMinutes = _Minutes;
}

int32 APrototype2Gamestate::GetMatchLengthSeconds()
{
	return MatchLengthSeconds;
}

int32 APrototype2Gamestate::GetCountdownLengthMinutes()
{
	return CountdownLengthMinutes;
}

int32 APrototype2Gamestate::GetCountdownLengthSeconds()
{
	return CountdownLengthSeconds;
}

int32 APrototype2Gamestate::GetBriefTimesUpLengthSeconds()
{
	return BriefTimesUpEndGameLengthSeconds;
}

int32 APrototype2Gamestate::GetPlayerWinner()
{
	return PlayerWinner;
}

int32 APrototype2Gamestate::GetWinningScore()
{
	return WinningScore;
}

int32 APrototype2Gamestate::GetSellMultiplier()
{
	return SellMultiplier;
}

void APrototype2Gamestate::SetSellMultiplier(int32 _Multiplier)
{
	SellMultiplier = _Multiplier;
}

void APrototype2Gamestate::VoteMap(EFarm _Level)
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
	}else if (_Level == EFarm::RANDOMFARM)
	{
		RandomFarm += 1;
	}
}

void APrototype2Gamestate::PickRandomMap()
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

void APrototype2Gamestate::AddCoinsTeams(APrototype2PlayerState* _Player, int32 _Amount)
{
	if (!HasAuthority())
		return;
	
	if (Server_TeamOne.Contains(_Player))
	{
		Team1Points += _Amount;
	}
	if (Server_TeamTwo.Contains(_Player))
	{
		Team2Points += _Amount;
	}
}

APrototype2Character* APrototype2Gamestate::GetWinningCharacter()
{
	APrototype2Character* OutWinningCharacter = nullptr;
	
	int32 HighestCoins{};
	for(auto PlayerState : Server_Players)
	{
		if (PlayerState->Coins > HighestCoins)
		{
			HighestCoins = PlayerState->Coins;

			if (auto SomePawn = PlayerState->GetPawn())
			{
				OutWinningCharacter = Cast<APrototype2Character>(SomePawn);
			}
		}
	}
	
	return OutWinningCharacter;
}

void APrototype2Gamestate::UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails)
{
	Server_Players[_Player]->Details = _CharacterDetails;
}

void APrototype2Gamestate::PupeteerCharactersForEndGame()
{
	AActor* EndGamePodiumActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEndGamePodium::StaticClass());
	AEndGamePodium* EndGamePodiumActorCasted = Cast<AEndGamePodium>(EndGamePodiumActor);

	EndGamePodiumActorCasted->PlayConfetteVFX();
	
	if (bTeams)
	{
		int32 Team1Coins{};
		int32 Team2Coins{};
		if (!PlayerArray.IsEmpty())
		{
			for(auto PlayerState : PlayerArray)
			{
				if (APrototype2PlayerState* CastedPlayerstate = Cast<APrototype2PlayerState>(PlayerState))
				{
					if (CastedPlayerstate->Details.Colour == TeamOneColour)
					{
						Team1Coins += CastedPlayerstate->Coins;
					}
					else if (CastedPlayerstate->Details.Colour == TeamTwoColour)
					{
						Team2Coins += CastedPlayerstate->Coins;
					}
				}
			}
		}

		if (EndGamePodiumActorCasted)
		{
			for(int32 i = 0; i < PlayerArray.Num(); i++)
			{
				if (APrototype2PlayerState* CastedPlayerState = Cast<APrototype2PlayerState>(PlayerArray[i]))
				{
					if (APrototype2Character* CastedPlayerCharacter = Cast<APrototype2Character>(CastedPlayerState->GetPlayerController()->GetCharacter()))
					{
						if (Team1Coins > Team2Coins && CastedPlayerState->Details.Colour == TeamOneColour)
							CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetWinPosition(i)->GetComponentTransform());
						else if (Team2Coins > Team1Coins && CastedPlayerState->Details.Colour == TeamTwoColour)
							CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetWinPosition(i)->GetComponentTransform());
						else if (Team2Coins == Team1Coins)
							CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetWinPosition(i)->GetComponentTransform());
						else
							CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetLossPosition(i)->GetComponentTransform());
					
						if (auto ControllerCast = Cast<APrototype2PlayerController>(CastedPlayerState->GetPlayerController()))
						{
							ControllerCast->SetViewTarget_Networked(EndGamePodiumActorCasted->EndGameCamera);
						}
					}
				}
			}
		}
	}
	else
	{
		int32 WinningPlayer{};
		int32 HighestCoins{};
		
		for(auto PlayerState : PlayerArray)
		{
			if (APrototype2PlayerState* CastedPlayerstate = Cast<APrototype2PlayerState>(PlayerState))
			{
				if (CastedPlayerstate->Coins > HighestCoins)
				{
					HighestCoins = CastedPlayerstate->Coins;
				}
			}
		}

		if (EndGamePodiumActorCasted)
		{
			for(int32 i = 0; i < PlayerArray.Num(); i++)
			{
				if (APrototype2PlayerState* CastedPlayerState = Cast<APrototype2PlayerState>(PlayerArray[i]))
				{
					if (APrototype2Character* CastedPlayerCharacter = Cast<APrototype2Character>(CastedPlayerState->GetPlayerController()->GetCharacter()))
					{
						if (HighestCoins > 0 && CastedPlayerState->Coins == HighestCoins)
							CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetWinPosition(WinningPlayer)->GetComponentTransform());
						else
							CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetLossPosition(i)->GetComponentTransform());
					
						if (auto ControllerCast = Cast<APrototype2PlayerController>(CastedPlayerState->GetPlayerController()))
						{
							ControllerCast->SetViewTarget_Networked(EndGamePodiumActorCasted->EndGameCamera);
						}
					}
				}
			}
		}
	}
}

void APrototype2Gamestate::SetGameTime()
{
	//
	if (!HasAuthority())
		return;

	if (!DefaultWorldOverrideData)
		return;
	
	switch (ExtraSettings.GameSpeed)
	{
	case SHORT_GAME:
		{
			MatchLengthMinutes = DefaultWorldOverrideData->GameTimeShort;
			break;
		}
	case MEDIUM_GAME:
		{
			MatchLengthMinutes = DefaultWorldOverrideData->GameTimeMedium;
			break;
		}
	case LONG_GAME:
		{
			MatchLengthMinutes = DefaultWorldOverrideData->GameTimeLong;
			break;
		}
	default:
		{
			MatchLengthMinutes = DefaultWorldOverrideData->GameTimeMedium;
			break;
		}
	}
}

void APrototype2Gamestate::UpdateTeamsScores()
{
	if (!bTeams)
		return;

	/* Update team 1 points */
	int32 Team1PointsTally = 0;
	for (int i = 0; i < Server_TeamOne.Num(); i++)
	{
		if (auto Player = Server_TeamOne[i])
		{
			Team1PointsTally += Player->Coins;
		}
		//UE_LOG(LogTemp, Warning, TEXT("T1Tally: %d"), Team1PointsTally);
	}
	Team1Points = Team1PointsTally;

	/* Update team 2 points */
	int32 Team2PointsTally = 0;
	for (int i = 0; i < Server_TeamTwo.Num(); i++)
	{
		if (auto Player = Server_TeamTwo[i])
		{
			Team2PointsTally += Player->Coins;
		}
	}
	Team2Points = Team2PointsTally;
}

void APrototype2Gamestate::TickMapRotationTimer(float _DeltaSeconds)
{
	if (!bHasGameFinished)
		return;

	if (MapRotationShowTimer < 0)
		return;
	
	MapRotationShowTimer -= _DeltaSeconds;

	if (MapRotationShowTimer <= 0 && bIsMapChoiceShowing == false)
		bIsMapChoiceShowing = true;

}

void APrototype2Gamestate::TickTimers(float _DeltaSeconds)
{
	if (HasAuthority())
	{
		if (bPreviousServerTravel != bShouldServerTravel)
		{
			bPreviousServerTravel = bShouldServerTravel;

			if (bShouldServerTravel)
			{
				bPreviousServerTravel = false;
				bShouldServerTravel = false;
			}
		}

		// Countdown between all players choosing map and actually starting
		if (bHasGameFinished && bIsMapChoiceShowing)
		{
			if (MapChoiceTotalLengthSeconds > 0)
			{
				MapChoiceTotalLengthSeconds -= _DeltaSeconds;
				UE_LOG(LogTemp, Warning, TEXT("Timer should be decreasing"));

				const int32 TotalVotes = Farm + WinterFarm + HoneyFarm + FloatingIslandFarm + ClockworkFarm + RandomFarm;
				if (TotalVotes == Server_Players.Num() && bMapChosen == false && bHasAllPlayersVoted == false)
				{
					bHasAllPlayersVoted = true;
					if (MapChoiceTotalLengthSeconds > MapChoiceLengthSeconds)
					{
						MapChoiceTotalLengthSeconds = MapChoiceLengthSeconds; 
					}
				}
			
				if (MapChoiceTotalLengthSeconds <= 0)
				{
					bIsCountingDown = false;

					PickMapToPlay();

					bCanTravel = true;
					//GetWorld()->ServerTravel(MapChoice, false, false); // Start level
				}
			}
		}
	}
}

void APrototype2Gamestate::PickMapToPlay()
{
	if (!GameInstanceRef)
		return;
	
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
		else if (ClockworkFarm > Farm && ClockworkFarm > WinterFarm && ClockworkFarm > HoneyFarm && ClockworkFarm > FloatingIslandFarm && ClockworkFarm >= RandomFarm) // floating islands farm gets most votes
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

			if (GameMode == 1) // Brawl
			{
				MapChoiceArray.RemoveAt(4); // Remove clockwork
				MapChoiceTopVotesArray.RemoveAt(4); // Remove 1 element
			}
			else if (GameMode == 2) // Blitz
			{
				MapChoiceArray.RemoveAt(4); // Remove honey farm
				MapChoiceArray.RemoveAt(2); // Remove clockwork
				MapChoiceTopVotesArray.RemoveAt(4); // Remove 1 element
				MapChoiceTopVotesArray.RemoveAt(3); // Remove 1 element
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

void APrototype2Gamestate::ServerTravel(float _DeltaSeconds)
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



