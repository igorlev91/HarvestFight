// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2GameMode.h"

#include "Prototype2Character.h"
#include "Prototype2PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

class ALobbyGamestate;

APrototype2GameMode::APrototype2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}

void APrototype2GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (auto gamestate = GetGameState<APrototype2Gamestate>())
	{
		GameStateRef = gamestate;
		gamestate->FinalConnectionCount = GetGameInstance<UPrototypeGameInstance>()->FinalConnectionCount;
		UE_LOG(LogTemp, Warning, TEXT("Final Connection Count : %s"), *FString::FromInt(gamestate->FinalConnectionCount));
	}
}

void APrototype2GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	//if (GetLocalRole() == ROLE_Authority && GetRemoteRole() >= ROLE_AutonomousProxy)
	
	if (HasAuthority())
	{
		if (auto* playerState = NewPlayer->GetPlayerState<APrototype2PlayerState>())
		{
			if (auto* gamestate = GetGameState<APrototype2Gamestate>())
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
				playerState->Player_ID = gamestate->Server_Players.Add(playerState);

				if (auto* character = Cast<APrototype2Character>(NewPlayer->GetCharacter()))
				{
					//character->GetMesh()->SetMaterial(0, PlayerMaterials[playerState->Player_ID]);
					//character->PlayerMat = PlayerMaterials[playerState->Player_ID];
					character->PlayerID = playerState->Player_ID;
					gamestate->MaxPlayersOnServer = GetGameInstance<UPrototypeGameInstance>()->MaxPlayersOnServer;
					gamestate->FinalConnectionCount = GetGameInstance<UPrototypeGameInstance>()->FinalConnectionCount;
					UE_LOG(LogTemp, Warning, TEXT("Final Connection Count: %s"), *FString::FromInt(gamestate->FinalConnectionCount));
					//character->Client_AddHUD();
					switch(playerState->Player_ID)
					{
					case 0:
						{
							character->SetActorLocation({1680.f,-70.f,90.f});
							break;
						}
					case 1:
						{
							character->SetActorLocation({-1910.000f,-60.000f,90.000f});
							break;
						}
					case 2:
						{
							character->SetActorLocation({-110.f,1730.f,90.f});
							break;
						}
					case 3:
						{
							character->SetActorLocation({-110.f,-1850.f,90.f});
							break;
						}
					default:
						character->SetActorLocation({1680.f,-70.f,90.f});
						break;
					}
				}
			}
		}
	}
}

void APrototype2GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void APrototype2GameMode::DisableControllerInput(APlayerController* PlayerController)
{
	if (PlayerController != nullptr)
	{
		PlayerController->DisableInput(PlayerController);
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
	}
}

void APrototype2GameMode::EnableControllerInput(APlayerController* PlayerController)
{
	if (PlayerController != nullptr)
	{
		PlayerController->EnableInput(PlayerController);
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
	}
}

void APrototype2GameMode::DisableControllerInputForAll()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (PlayerController != nullptr)
		{
			DisableControllerInput(PlayerController);
		}
	}
}

void APrototype2GameMode::EnableControllerInputForAll()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (PlayerController != nullptr)
		{
			EnableControllerInput(PlayerController);
		}
	}
}

