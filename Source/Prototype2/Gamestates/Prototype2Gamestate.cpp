#include "Prototype2Gamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Prototype2PlayerController.h"
#include "Prototype2/Prototype2PlayerState.h"

APrototype2Gamestate::APrototype2Gamestate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2Gamestate::BeginPlay()
{
	Super::BeginPlay();
}

void APrototype2Gamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickCountdownTimer(DeltaSeconds);
	TickMatchTimer(DeltaSeconds);
	TickEndGameTimer(DeltaSeconds);
}

void APrototype2Gamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthSeconds);
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
}

void APrototype2Gamestate::TickCountdownTimer(float DeltaSeconds)
{
	if (HasAuthority() && !bGameHasStarted)
	{
		if (Server_Players.Num() >= FinalConnectionCount && !bGameHasStarted)
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
			if (CountdownLengthSeconds <= 0)
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
	if (HasAuthority() && bGameHasStarted)
	{
		if (CountdownLengthSeconds > 0)
		{
			CountdownLengthSeconds -= DeltaSeconds;
		}
		if (bPreviousServerTravel != bShouldServerTravel)
		{
			bPreviousServerTravel = bShouldServerTravel;

			if (bShouldServerTravel)
			{
				if (MatchLengthSeconds > 0 || MatchLengthMinutes > 0)
				{
					bIsCountingDown = true;
				}
				bPreviousServerTravel = false;
				bShouldServerTravel = false;
			}
		}

		if (bIsCountingDown)
		{
			if (MatchLengthSeconds > 0)
			{
				MatchLengthSeconds -= DeltaSeconds;
			}
			else
			{
				if (MatchLengthMinutes <= 0 && MatchLengthSeconds <= BriefTimesUpEndGameLengthSeconds)
				{
					bHasGameFinished = true;
					bIsCountingDown = false;
				}
				else
				{
					MatchLengthMinutes--;
					MatchLengthSeconds = 60;
				}
			}
		}
	}
}

void APrototype2Gamestate::TickEndGameTimer(float DeltaSeconds)
{
	// check if game has "finished"
	if (bHasGameFinished)
	{
		bIsCountingDown = false;
		MatchLengthMinutes = 0;
		MatchLengthSeconds = 0.0f;
		CountdownLengthSeconds = 0.0f;
		CountdownLengthMinutes = 0;
		
		if (BriefTimesUpEndGameLengthSeconds > 0.0f)
		{
			BriefTimesUpEndGameLengthSeconds -= DeltaSeconds;
		}
		else
		{
			if (EndGameLengthSeconds > 0)
			{
				EndGameLengthSeconds -= DeltaSeconds;
			}
			else
			{
				if (EndGameLengthSeconds <= 0.0f)
				{
					bGameReadyForVote = true;
				}
				else
				{
					EndGameLengthMinutes--;
					EndGameLengthSeconds = 60;
				}
			}
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

