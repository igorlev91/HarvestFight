// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerState.h"

#include "Net/UnrealNetwork.h"

void APrototype2PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2PlayerState, Player_ID);
	DOREPLIFETIME(APrototype2PlayerState, Coins);
	DOREPLIFETIME(APrototype2PlayerState, ExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, IsShowingExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, MaxTimeShowExtraCoins);
	DOREPLIFETIME(APrototype2PlayerState, TimerExtraCoins);

	DOREPLIFETIME(APrototype2PlayerState, Character);
	DOREPLIFETIME(APrototype2PlayerState, CharacterColour);
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

void APrototype2PlayerState::GrabSkinFromGameInstance()
{
	if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		Character = gameInstance->Character;
		CharacterColour = gameInstance->CharacterColour;
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character: %s"), *FString::FromInt((int)gameInstance->Character));
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character Colour: %s"), *FString::FromInt((int)gameInstance->CharacterColour));
	}

	Server_GrabSkinFromGameInstance(Character, CharacterColour);
}

void APrototype2PlayerState::Server_GrabSkinFromGameInstance_Implementation(ECharacters _character, ECharacterColours _colour)
{
	Multi_GrabSkinFromGameInstance(_character, _colour);
}

void APrototype2PlayerState::Multi_GrabSkinFromGameInstance_Implementation(ECharacters _character,
	ECharacterColours _colour)
{
	Character = _character;
	CharacterColour = _colour;
	UE_LOG(LogTemp, Warning, TEXT("Multi - GameInstance Character: %s"), *FString::FromInt((int)_character));
	UE_LOG(LogTemp, Warning, TEXT("Multi - GameInstance Character Colour: %s"), *FString::FromInt((int)_colour));
}

void APrototype2PlayerState::UpdateCharacterMaterial(ECharacters _character, ECharacterColours _characterColour)
{
	Character = _character;
	CharacterColour = _characterColour;
}
