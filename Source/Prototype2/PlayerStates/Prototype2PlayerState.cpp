// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerState.h"
#include "Prototype2/Pickups/Plant.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/PoisonFertiliserData.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Gameplay/RandomEventManager.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

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

	DOREPLIFETIME(APrototype2PlayerState, Emote);
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
	Gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
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

void APrototype2PlayerState::AddCoins(int32 _amount)
{
	bIsShowingExtraCoins = true;
	ExtraCoins = _amount;
	Client_OnAddCoins();
	Multi_OnAddCoins();
}

void APrototype2PlayerState::AddCoins(APlant* _SomePlant)
{
	bIsShowingExtraCoins = true;
	int32 PlantSellValue = (_SomePlant->SeedData->BabyStarValue * _SomePlant->SeedData->PlantData->Multiplier * (_SomePlant->NumberOfNearbyFlowers + 1));
	PlantSellValue *= Gamestate->SellMultiplier;
	ExtraCoins = _SomePlant->ItemComponent->bGold ? PlantSellValue * _SomePlant->SeedData->GoldMultiplier : PlantSellValue;
	Client_OnAddCoins();
	Multi_OnAddCoins();
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

void APrototype2PlayerState::Client_OnAddCoins()
{
	OnItemSoldDelegate.Broadcast(Player_ID);
}

void APrototype2PlayerState::Multi_OnAddCoins()
{
	OnItemSoldDelegate.Broadcast(Player_ID);
}

bool APrototype2PlayerState::IsLoosing()
{
	bool bLoosing{};
	int32 LoosingPlayerScore{9999};
	if (Gamestate)
	{
		for(APrototype2PlayerState* Player : Gamestate->Server_Players)
		{
			if (Player->Coins < LoosingPlayerScore)
			{
				LoosingPlayerScore = Player->Coins;
			}
		}
	}
	if (Coins == LoosingPlayerScore)
		bLoosing = true;
	
	return bLoosing;
}

bool APrototype2PlayerState::IsWinning()
{
	bool bWinning{};
	int32 WinningPlayerScore{};
	if (Gamestate)
	{
		for(APrototype2PlayerState* Player : Gamestate->Server_Players)
		{
			if (Player->Coins > WinningPlayerScore)
			{
				WinningPlayerScore = Player->Coins;
			}
		}
	}
	if (Coins == WinningPlayerScore)
		bWinning = true;
	
	return bWinning;
}

void APrototype2PlayerState::UpdateCharacterMaterial(FCharacterDetails _Details)
{
	Details = _Details;
}
