// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2GameMode.h"

#include "Prototype2Character.h"
#include "Prototype2PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

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
				UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
				playerState->Player_ID = gamestate->Server_Players.Add(playerState);

				switch(playerState->Player_ID)
				{
				case 0:
					{
						NewPlayer->GetCharacter()->SetActorLocation({1680.f,-70.f,90.f});
						break;
					}
				case 1:
					{
						NewPlayer->GetCharacter()->SetActorLocation({-1910.000f,-60.000f,90.000f});
						break;
					}
				case 2:
					{
						NewPlayer->GetCharacter()->SetActorLocation({-110.f,1730.f,90.f});
						break;
					}
				case 3:
					{
						NewPlayer->GetCharacter()->SetActorLocation({-110.f,-1850.f,90.f});
						break;
					}
				default:
					break;
				}
			}
		}
	}
}


