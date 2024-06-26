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
	//DOREPLIFETIME(APrototype2PlayerState, ExtraCoins);

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
	
	Gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
}



void APrototype2PlayerState::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	//if (bIsShowingExtraCoins == true)
	//{
	//	TimerExtraCoins -= _DeltaSeconds;
//
	//	if (TimerExtraCoins <= 0)
	//	{
	//		//Coins += ExtraCoins;
	//		TimerExtraCoins = MaxTimeShowExtraCoins; // Reset timer to max time
	//		bIsShowingExtraCoins = false;
	//	}
	//}
}

void APrototype2PlayerState::VoteMap(EFarm _Map)
{
	if (HasAuthority())
	{
		APrototype2Gamestate* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
		if (IsValid(GameState))
		{
			GameState->VoteMap(_Map);
		}
	}
	else
	{
		Server_VoteMap(_Map);
	}
}

void APrototype2PlayerState::Server_VoteMap_Implementation(EFarm _Map)
{
	APrototype2Gamestate* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState))
	{
		GameState->VoteMap(_Map);
	}
}

void APrototype2PlayerState::AddCoins(int32 _amount)
{
	//bIsShowingExtraCoins = true;
	Coins += _amount;
	//ExtraCoins = _amount;

	OnRep_Coins();

	APrototype2Gamestate* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState))
	{
		GameState->AddCoinsTeams(this, _amount);
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

void APrototype2PlayerState::Client_OnAddCoins(int32 _Score)
{
	OnItemSoldDelegate.Broadcast(Player_ID, _Score);
}

void APrototype2PlayerState::Multi_OnAddCoins(int32 _Score)
{
	OnItemSoldDelegate.Broadcast(Player_ID, _Score);
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

void APrototype2PlayerState::OnRep_Coins()
{
	if (Coins == LastLocalCoins)
		return;
	
	auto LocalPlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(LocalPlayerController) == false)
		return;

	if (LocalPlayerController->IsLocalController() == false)
		return;

	if (GetWorld()->GetRealTimeSeconds() <= 5.0f)
		return;

	auto LocalPlayerState = LocalPlayerController->GetPlayerState<APrototype2PlayerState>();
	if (IsValid(LocalPlayerState) == false)
		return;
	
	APrototype2Gamestate* GamestateCast = GetWorld()->GetGameState<APrototype2Gamestate>();
	if (IsValid(GamestateCast) == false)
		return;
	
	AActor* SellBinActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASellBin::StaticClass());
	if (IsValid(SellBinActor) == false)
		return;

	ExtraCoinsLocal = Coins - LastLocalCoins;
	
	auto SellBinCast = Cast<ASellBin>(SellBinActor);
	if (GamestateCast->Server_Players.Contains(this))
	{
		SellBinCast->OnItemSoldDelegate.Broadcast(GamestateCast->Server_Players.Find(this), ExtraCoinsLocal);
	}

	LastLocalCoins = Coins;
}

void APrototype2PlayerState::UpdateCharacterMaterial(FCharacterDetails _Details)
{
	Details = _Details;
}
