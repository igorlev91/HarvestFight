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

