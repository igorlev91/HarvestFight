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
	DOREPLIFETIME(APrototype2Gamestate, GameHasStarted);
	DOREPLIFETIME(APrototype2Gamestate, HasGameFinished);
	DOREPLIFETIME(APrototype2Gamestate, GameReadyForVote);

	DOREPLIFETIME(APrototype2Gamestate, EndGameLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, EndGameLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, BriefTimesUpEndGameLengthSeconds);
	
	DOREPLIFETIME(APrototype2Gamestate, IsCountingDown);
	DOREPLIFETIME(APrototype2Gamestate, PreviousServerTravel);
	
	DOREPLIFETIME(APrototype2Gamestate, Server_Players);
	
	DOREPLIFETIME(APrototype2Gamestate, MaxPlayersOnServer);
	DOREPLIFETIME(APrototype2Gamestate, FinalConnectionCount);
}

void APrototype2Gamestate::TickCountdownTimer(float DeltaSeconds)
{
	if (HasAuthority() && !GameHasStarted)
	{
		if (Server_Players.Num() >= FinalConnectionCount && !GameHasStarted)
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
				GameHasStarted = true;
				UE_LOG(LogTemp, Warning, TEXT("Countdown completed"));
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( GameHasStarted ? TEXT("true") : TEXT("false") ));
	}
}

void APrototype2Gamestate::TickMatchTimer(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( GameHasStarted ? TEXT("true") : TEXT("false") ));
	if (HasAuthority() && GameHasStarted)
	{
		if (CountdownLengthSeconds > 0)
		{
			CountdownLengthSeconds -= DeltaSeconds;
		}
		if (PreviousServerTravel != ShouldServerTravel)
		{
			PreviousServerTravel = ShouldServerTravel;

			if (ShouldServerTravel)
			{
				if (MatchLengthSeconds > 0 || MatchLengthMinutes > 0)
				{
					IsCountingDown = true;
				}
				PreviousServerTravel = false;
				ShouldServerTravel = false;
			}
		}

		if (IsCountingDown)
		{
			if (MatchLengthSeconds > 0)
			{
				MatchLengthSeconds -= DeltaSeconds;
			}
			else
			{
				if (MatchLengthMinutes <= 0 && MatchLengthSeconds <= BriefTimesUpEndGameLengthSeconds)
				{
					HasGameFinished = true;
					IsCountingDown = false;
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
	if (HasGameFinished)
	{
		IsCountingDown = false;
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
					GameReadyForVote = true;
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

