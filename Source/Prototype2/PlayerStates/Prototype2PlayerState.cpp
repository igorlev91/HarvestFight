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
		Character = GameInstance->Character;
		CharacterColour = GameInstance->CharacterColour;
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character: %s"), *FString::FromInt((int32)GameInstance->Character));
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character Colour: %s"), *FString::FromInt((int32)GameInstance->CharacterColour));
	}

	Server_GrabSkinFromGameInstance(Character, CharacterColour);
}

void APrototype2PlayerState::Server_GrabSkinFromGameInstance_Implementation(ECharacters _Character, ECharacterColours _Colour)
{
	Multi_GrabSkinFromGameInstance(_Character, _Colour);
}

void APrototype2PlayerState::Multi_GrabSkinFromGameInstance_Implementation(ECharacters _Character, ECharacterColours _Colour)
{
	Character = _Character;
	CharacterColour = _Colour;
	UE_LOG(LogTemp, Warning, TEXT("Multi - GameInstance Character: %s"), *FString::FromInt((int32)_Character));
	UE_LOG(LogTemp, Warning, TEXT("Multi - GameInstance Character Colour: %s"), *FString::FromInt((int32)_Colour));
}

void APrototype2PlayerState::UpdateCharacterMaterial(ECharacters _Character, ECharacterColours _Colour)
{
	Character = _Character;
	CharacterColour = _Colour;
}
