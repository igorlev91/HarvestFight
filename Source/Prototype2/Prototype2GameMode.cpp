// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2GameMode.h"
#include "Prototype2Character.h"
#include "UObject/ConstructorHelpers.h"

APrototype2GameMode::APrototype2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}

void APrototype2GameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup timer start
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APrototype2GameMode::Countdown, 1.0f, true, 0.0f);
}

void APrototype2GameMode::Countdown()
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
