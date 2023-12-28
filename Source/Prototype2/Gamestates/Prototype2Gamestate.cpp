#include "Prototype2Gamestate.h"


void APrototype2Gamestate::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup timer start
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APrototype2Gamestate::Countdown, 1.0f, true, 0.0f);
}

void APrototype2Gamestate::Countdown()
{
	if (Seconds != 0)
	{
		Seconds--;
	}
	else
	{
		if (Minutes == 0)
		{
			// End of timer
		}
		else
		{
			Minutes--;
			Seconds = 59;
		}
	}
}