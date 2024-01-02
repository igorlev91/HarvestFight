// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerController.h"

#include "Prototype2GameMode.h"
#include "Blueprint/UserWidget.h"
#include "Gamestates/LobbyGamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Widget_PlayerHUD.h"

void APrototype2PlayerController::BeginPlay()
{
	Super::BeginPlay();


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



void APrototype2PlayerController::KickFromLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMenu")), true, "kicked");
}
