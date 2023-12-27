// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2GameMode.generated.h"

UCLASS(minimalapi)
class APrototype2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrototype2GameMode();

	virtual void BeginPlay() override;

	
	// Timer
	UPROPERTY(EditAnywhere)
	int Minutes{5};

	UPROPERTY(EditAnywhere)
	int Seconds;

public:
	void Countdown();
};



