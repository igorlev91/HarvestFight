#include "LobbyGamestate.h"

ALobbyGamestate::ALobbyGamestate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALobbyGamestate::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(LobbyTimerHandle, this, &ALobbyGamestate::CountdownLobbyTimer, 1.0f, true, 0.0f);
	GetWorldTimerManager().PauseTimer(LobbyTimerHandle);
	
}

void ALobbyGamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PreviousServerTravel != ShouldServerTravel)
	{
		PreviousServerTravel = ShouldServerTravel;

		if (ShouldServerTravel)
		{
			if (LobbyLengthSeconds > 0 || LobbyLengthMinutes > 0)
			{
				GetWorldTimerManager().UnPauseTimer(LobbyTimerHandle);
			}
			PreviousServerTravel = false;
			ShouldServerTravel = false;
		}
	}
}

void ALobbyGamestate::CountdownLobbyTimer()
{
	if (LobbyLengthSeconds > 0)
	{
		LobbyLengthSeconds--;
	}
	else
	{
		if (LobbyLengthMinutes <= 0)
		{
			// End of timer
			GetWorldTimerManager().PauseTimer(LobbyTimerHandle);
			GetWorld()->ServerTravel("Level_Main");
		}
		else
		{
			LobbyLengthMinutes--;
			LobbyLengthSeconds = 59;
		}
	}
}
