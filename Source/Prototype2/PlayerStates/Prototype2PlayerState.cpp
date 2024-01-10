// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerState.h"

#include "Net/UnrealNetwork.h"

void APrototype2PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2PlayerState, Player_ID);
	DOREPLIFETIME(APrototype2PlayerState, Coins);
	DOREPLIFETIME(APrototype2PlayerState, ExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, bIsShowingExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, MaxTimeShowExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, TimerExtraCoins);

	DOREPLIFETIME(APrototype2PlayerState, Details);

	DOREPLIFETIME(APrototype2PlayerState, PlayerName);
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



void APrototype2PlayerState::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	if (bIsShowingExtraCoins == true)
	{
		TimerExtraCoins -= _DeltaSeconds;

		if (TimerExtraCoins <= 0)
		{
			Coins += ExtraCoins;
			TimerExtraCoins = MaxTimeShowExtraCoins; // Reset timer to max time
			bIsShowingExtraCoins = false;
		}
	}
}

void APrototype2PlayerState::GrabSkinFromGameInstance()
{
	if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		Details = GameInstance->FinalPlayerDetails[GameInstance->UniqueNetIDName];
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character: %s"), *FString::FromInt((int32)GameInstance->FinalPlayerDetails[GameInstance->UniqueNetIDName].Character));
		//UE_LOG(LogTemp, Warning, TEXT("GameInstance Character Colour: %s"), *FString::FromInt((int32)GameInstance->FinalPlayerDetails[GameInstance->UniqueNetIDName].CharacterColour));
	}

	Server_GrabSkinFromGameInstance(Details);
}

void APrototype2PlayerState::Server_GrabSkinFromGameInstance_Implementation(FCharacterDetails _Details)
{
	Multi_GrabSkinFromGameInstance(_Details);
}

void APrototype2PlayerState::Multi_GrabSkinFromGameInstance_Implementation(FCharacterDetails _Details)
{
	Details = _Details;
	UE_LOG(LogTemp, Warning, TEXT("Multi - GameInstance Character: %s"), *FString::FromInt((int32)_Details.Character));
	//UE_LOG(LogTemp, Warning, TEXT("Multi - GameInstance Character Colour: %s"), *FString::FromInt((int32)_Colour));
}

void APrototype2PlayerState::UpdateCharacterMaterial(FCharacterDetails _Details)
{
	Details = _Details;
}
