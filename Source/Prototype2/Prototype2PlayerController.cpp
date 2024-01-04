// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerController.h"

#include "LobbyPlayerState.h"
#include "Prototype2GameMode.h"
#include "Prototype2PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Gamemodes/LobbyGamemode.h"
#include "Gamestates/LobbyGamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyCharacter.h"
#include "Prototype2/EndGameCamera.h"
#include "Prototype2/EndGamePodium.h"
#include "Widgets/Widget_PlayerHUD.h"
#include "Prototype2//Gamestates/Prototype2Gamestate.h"

void APrototype2PlayerController::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
	if (auto gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gamestate;
	}

	bCanPossessWithoutAuthority = true;
}

void APrototype2PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GameStateRef)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("HasGameStarted? : %s"), *FString::FromInt((int)GameStateRef->GameHasStarted));
	
	if (GameStateRef->HasGameFinished)
	{
		if (auto gamemode = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			if (auto endGamePodium = gamemode->EndGamePodium)
			{
				if (auto endGameCamera = endGamePodium->EndGameCamera)
				{
					SetViewTarget(endGameCamera);
					UnPossess();
					SetViewTarget(endGameCamera);
				}
			}
		}
	}

	if (!GameStateRef->GameHasStarted || GameStateRef->HasGameFinished)
	{
		DisableInput(this);
		SetIgnoreLookInput(true);
		SetIgnoreMoveInput(true);
	}
	else
	{
		EnableInput(this);
		SetIgnoreLookInput(false);
		SetIgnoreMoveInput(false);
	}

	if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		if (auto playerState = GetPlayerState<APrototype2PlayerState>())
		{
			UpdateCharacterMaterial(playerState->Player_ID, gameInstance->Character, gameInstance->CharacterColour);
		}
	}
}

void APrototype2PlayerController::SetIsReady(int _player, bool _isReady)
{
	Server_SetIsReady(_player, _isReady);
}

void APrototype2PlayerController::Server_SetIsReady_Implementation(int _player, bool _isReady)
{
	if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		gameState->SetIsReady(_player, _isReady);
	}
}

void APrototype2PlayerController::VoteMap(int _player, EFarm _level)
{
	Server_VoteMap(_player, _level);
}


void APrototype2PlayerController::Server_VoteMap_Implementation(int _player, EFarm _level)
{
	if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		gameState->VoteMap(_player, _level);
	}
}

void APrototype2PlayerController::UpdateCharacterMaterial(int _player, ECharacters _character, ECharacterColours _characterColour)
{
	Server_UpdateCharacterMaterial(_player, _character, _characterColour);
}

void APrototype2PlayerController::Server_UpdateCharacterMaterial_Implementation(int _player, ECharacters _character,
	ECharacterColours _characterColour)
{
	if (auto gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		gameState->UpdateCharacterMaterial(_player, _character, _characterColour);
	}
	else if (auto protoGameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		protoGameState->UpdateCharacterMaterial(_player, _character, _characterColour);
	}
}

void APrototype2PlayerController::KickFromLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMenu")), true, "kicked");
}
