// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerController.h"

#include "LobbyPlayerState.h"
#include "Prototype2GameMode.h"
#include "Blueprint/UserWidget.h"
#include "Gamemodes/LobbyGamemode.h"
#include "Gamestates/LobbyGamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyCharacter.h"
#include "Widgets/Widget_PlayerHUD.h"

void APrototype2PlayerController::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Server_UpdateMaterial();
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
	if (auto* gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		gameState->UpdateCharacterMaterial(_player, _character, _characterColour);
	}
}

void APrototype2PlayerController::KickFromLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMenu")), true, "kicked");
}
