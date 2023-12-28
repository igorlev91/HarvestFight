#include "Prototype2Gamestate.h"


APrototype2Gamestate::APrototype2Gamestate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2Gamestate::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &APrototype2Gamestate::CountdownMatchTimer, 1.0f, true, 0.0f);
	GetWorldTimerManager().PauseTimer(MatchTimerHandle);
	
}

void APrototype2Gamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PreviousServerTravel != ShouldServerTravel)
	{
		PreviousServerTravel = ShouldServerTravel;

		if (ShouldServerTravel)
		{
			if (MatchLengthSeconds > 0 || MatchLengthMinutes > 0)
			{
				GetWorldTimerManager().UnPauseTimer(MatchTimerHandle);
			}
			PreviousServerTravel = false;
			ShouldServerTravel = false;
		}
	}
}

void APrototype2Gamestate::CountdownMatchTimer()
{
	if (MatchLengthSeconds > 0)
	{
		MatchLengthSeconds--;
	}
	else
	{
		if (MatchLengthMinutes <= 0)
		{
			// End of timer
			GetWorldTimerManager().PauseTimer(MatchTimerHandle);
			if (HasAuthority())
				GetWorld()->ServerTravel("Level_MainMenu");
		}
		else
		{
			MatchLengthMinutes--;
			MatchLengthSeconds = 59;
		}
	}
}


