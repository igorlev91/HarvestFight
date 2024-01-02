// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerState.h"

#include "Net/UnrealNetwork.h"

void APrototype2PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2PlayerState, Coins);
	DOREPLIFETIME(APrototype2PlayerState, ExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, IsShowingExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, MaxTimeShowExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, TimerExtraCoins);
}

APrototype2PlayerState::APrototype2PlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2PlayerState::BeginPlay()
{
	Super::BeginPlay();

	TimerExtraCoins = MaxTimeShowExtraCoins; // Preset timer to max time
}

void APrototype2PlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsShowingExtraCoins == true)
	{
		TimerExtraCoins -= DeltaSeconds;

		if (TimerExtraCoins <= 0)
		{
			Coins += ExtraCoins;
			TimerExtraCoins = MaxTimeShowExtraCoins; // Reset timer to max time
			IsShowingExtraCoins = false;
		}
	}
}
