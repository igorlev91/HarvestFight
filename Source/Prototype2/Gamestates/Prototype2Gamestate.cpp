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

	if (HasAuthority())
	{
		if (Server_Players.Num() < FinalConnectionCount && !GameHasStarted)
		{
		}
		else if (Server_Players.Num() >= FinalConnectionCount && !GameHasStarted)
		{
			if (CountdownLengthSeconds > 0)
			{
				CountdownLengthSeconds -= DeltaSeconds;
				
			}
			else
			{
				if (CountdownLengthSeconds <= 7.0f)
				{
					GameHasStarted = true;
				}
				else
				{
					CountdownLengthMinutes--;
					CountdownLengthSeconds = 60;
				}
			}
		}
	}

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
				if (MatchLengthMinutes <= 0 && MatchLengthSeconds <= 0)
				{
					// End of timer
					//GetWorld()->ServerTravel("Level_MainMenu");
					HasGameFinished = true;
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

void APrototype2Gamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, CountdownLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, CountdownLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, GameHasStarted);
	DOREPLIFETIME(APrototype2Gamestate, HasGameFinished);
	DOREPLIFETIME(APrototype2Gamestate, IsCountingDown);
	DOREPLIFETIME(APrototype2Gamestate, PreviousServerTravel);
	
	DOREPLIFETIME(APrototype2Gamestate, Server_Players);
	
	DOREPLIFETIME(APrototype2Gamestate, MaxPlayersOnServer);
	DOREPLIFETIME(APrototype2Gamestate, FinalConnectionCount);
}

