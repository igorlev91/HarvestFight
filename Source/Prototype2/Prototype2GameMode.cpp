// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2GameMode.h"

#include "EndGamePodium.h"
#include "Prototype2Character.h"
#include "Prototype2PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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
	
	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (auto gamestate = GetGameState<APrototype2Gamestate>())
	{
		GameStateRef = gamestate;
		gamestate->FinalConnectionCount = GetGameInstance<UPrototypeGameInstance>()->FinalConnectionCount;
		//UE_LOG(LogTemp, Warning, TEXT("Final Connection Count : %s"), *FString::FromInt(gamestate->FinalConnectionCount));
	}

	if (EndGamePodiumPrefab)
		EndGamePodium = GetWorld()->SpawnActor<AEndGamePodium>(EndGamePodiumPrefab, FVector{3031.58f,-1426.65f,-17.30},FRotator{0.0f,140.59f,0.0f});
}

void APrototype2GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		if (auto playerState = NewPlayer->GetPlayerState<APrototype2PlayerState>())
		{
			if (auto gamestate = GetGameState<APrototype2Gamestate>())
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
				playerState->Player_ID = gamestate->Server_Players.Add(playerState);

				if (auto* character = Cast<APrototype2Character>(NewPlayer->GetCharacter()))
				{
					if (PlayerMaterials.Num() > (int)playerState->Character * 3 + (int)playerState->CharacterColour)
					{
						character->PlayerMat = PlayerMaterials[(int)playerState->Character * 3 + (int)playerState->CharacterColour];
					}

					character->PlayerID = playerState->Player_ID;
					gamestate->MaxPlayersOnServer = GetGameInstance<UPrototypeGameInstance>()->MaxPlayersOnServer;
					gamestate->FinalConnectionCount = GetGameInstance<UPrototypeGameInstance>()->FinalConnectionCount;
					character->PlayerStateRef = playerState;
					
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
	
	LookOutForGameEnd();
	
	if (auto gamestate = GetGameState<APrototype2Gamestate>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Gamestate"));
		for(auto i = 0; i < gamestate->Server_Players.Num(); i++)
		{
			if (auto playerState = gamestate->Server_Players[i])
			{
				if (auto controller = playerState->GetPlayerController())
				{
					//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Player Controller"));
					if (auto character = Cast<APrototype2Character>(controller->GetCharacter()))
					{
						//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Player Character"));
						if (PlayerMaterials.Num() > (int)playerState->CharacterColour)
						{
							//UE_LOG(LogTemp, Warning, TEXT("Gamemode: Set Player Material"));
							if (PlayerMaterials.Num() > (int)playerState->Character * 4 + (int)playerState->CharacterColour)
								character->PlayerMat = PlayerMaterials[(int)playerState->Character * 4 + (int)playerState->CharacterColour];
							else
								character->PlayerMat = PlayerMaterials[(int)playerState->CharacterColour];
						}
					}
				}
			}
		}
	}
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

void APrototype2GameMode::LookOutForGameEnd()
{
	if (GameStateRef)
	{
		if (Server_Characters.Num() < GameStateRef->Server_Players.Num())
		{
			Server_Characters.Empty();
			Server_Characters = {};
			for(auto playerState : GameStateRef->Server_Players)
			{
				if (auto* character = Cast<APrototype2Character>(playerState->GetPlayerController()->GetCharacter()))
				{
					Server_Characters.Add(character);
				}
			}
		}

		if (Server_PlayerStates.Num() < GameStateRef->Server_Players.Num())
			Server_PlayerStates = GameStateRef->Server_Players;
		
		if (GameStateRef->HasGameFinished && HasAuthority() && !TpHasHappened)
		{
			Multi_TeleportEveryoneToPodium();
			TpHasHappened = true;
		}
	}
}

void APrototype2GameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2GameMode, EndGamePodium);
}

void APrototype2GameMode::Multi_TeleportEveryoneToPodium_Implementation()
{
	APrototype2Character* firstPlace{nullptr};
	APrototype2Character* secondPlace{nullptr};
	APrototype2Character* thirdPlace{nullptr};
	int highestCoins{};
	int secondHighestCoins{};
			
	for(int i = 0; i < Server_PlayerStates.Num(); i++)
	{
		if (auto* character = Cast<APrototype2Character>(Server_Characters[i]))
		{
			if (Server_PlayerStates[i].Get()->Coins > highestCoins)
			{
				highestCoins = Server_PlayerStates[i].Get()->Coins;
				firstPlace = character;
			}
			else if (Server_PlayerStates[i].Get()->Coins > secondHighestCoins)
			{
				secondHighestCoins = Server_PlayerStates[i].Get()->Coins;
				secondPlace = character;
			}
			else
			{
				thirdPlace = character;
			}
		}
	}

	if (auto gamemode = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (auto endGamePodium = gamemode->EndGamePodium)
		{
			FTransform defautTransform{};
			defautTransform.SetScale3D({1.0f, 1.0f, 1.0f});
			// Tp Everyone
			if (firstPlace)
			{
				firstPlace->AttachToComponent(endGamePodium->P1Position,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				firstPlace->SetActorRelativeTransform(defautTransform);
			}
			if (secondPlace)
			{
				secondPlace->AttachToComponent(endGamePodium->P2Position,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				secondPlace->SetActorRelativeTransform(defautTransform);
			}
			if (thirdPlace)
			{
				thirdPlace->AttachToComponent(endGamePodium->P3Position,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				thirdPlace->SetActorRelativeTransform(defautTransform);
			}
		}
	}
}

