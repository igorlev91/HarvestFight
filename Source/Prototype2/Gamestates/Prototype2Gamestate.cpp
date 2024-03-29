

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

	if (GameModeRef)
		return;
	
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	GameModeRef = Cast<APrototype2GameMode>(GameMode);
}

void APrototype2Gamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TickCountdownTimer(DeltaSeconds);
	TickMatchTimer(DeltaSeconds);
	TickEndGameTimer(DeltaSeconds);

	UpdateTeamsScores();
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

void APrototype2Gamestate::UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails)
{
	Server_Players[_Player]->Details = _CharacterDetails;
}

void APrototype2Gamestate::PupeteerCharactersForEndGame()
{
	AActor* EndGamePodiumActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEndGamePodium::StaticClass());
	AEndGamePodium* EndGamePodiumActorCasted = Cast<AEndGamePodium>(EndGamePodiumActor);

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

