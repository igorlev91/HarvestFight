// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2GameMode.h"

#include "EndGamePodium.h"
#include "Prototype2Character.h"
#include "Prototype2PlayerController.h"
#include "RadialPlot.h"
#include "SellBin_Winter.h"
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
	{
		EndGamePodium = GetWorld()->SpawnActor<AEndGamePodium>(EndGamePodiumPrefab, FVector{3031.58f,-1426.65f,-17.30},FRotator{0.0f,140.59f,0.0f});
		EndGamePodium->SetReplicates(true);
		EndGamePodium->SetReplicatingMovement(true);
	}
	
	if (SellBinPrefab)
	{
		SellBinRef = GetWorld()->SpawnActor<ASellBin>(SellBinPrefab, {-104.559325,-72.190911,-30.0f},FRotator::ZeroRotator);
		SellBinRef->SetReplicates(true);
		SellBinRef->SetReplicatingMovement(true);
		if (auto winterSellBin = Cast<ASellBin_Winter>(SellBinRef))
		{
			winterSellBin->SetActorLocation({-104.559325,-72.190911,-13.473242});
		}
		Multi_DetachShippingBinComponents();
	}
		
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

				if (auto character = Cast<APrototype2Character>(NewPlayer->GetCharacter()))
				{
					character->SetPlayerState(playerState);
					
					character->PlayerID = playerState->Player_ID;
					character->PlayerStateRef = playerState;
					if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
					{
						gamestate->MaxPlayersOnServer = gameInstance->MaxPlayersOnServer;
						gamestate->FinalConnectionCount = gameInstance->FinalConnectionCount;

						if (gameInstance->FinalPlayerNames.Num() > 0)
						{
							for(int i = 0; i < gameInstance->FinalPlayerNames.Num(); i++)
							{
								FString newPlayerName;
								IOnlineIdentityPtr IdentityInterface = IOnlineSubsystem::Get()->GetIdentityInterface();
								if (IdentityInterface.IsValid())
								{
									newPlayerName = IdentityInterface->GetPlayerNickname(playerState->Player_ID);
									UE_LOG(LogTemp, Warning, TEXT("Player %s Has Steam Name %s"), *FString::FromInt(playerState->Player_ID), *newPlayerName);
								}
								if (gameInstance->FinalPlayerNames[i] == newPlayerName)
								{
									if (gameInstance->FinalCharacters.Num() > i)
										playerState->Character = gameInstance->FinalCharacters[i];
									if (gameInstance->FinalColours.Num() > i)
										playerState->CharacterColour = gameInstance->FinalColours[i];
								}
							}
						}

						bool duplicateSkin{};
						for(int i = 0; i < gameInstance->FinalPlayerNames.Num() && duplicateSkin == false; i++)
						{
							for(int j = 0; j < gameInstance->FinalPlayerNames.Num() && duplicateSkin == false; j++)
							{
								if (j != i)
								{
									if (gameInstance->FinalPlayerNames[i] == gameInstance->FinalPlayerNames[j])
									{
										duplicateSkin = true;
									}
								}
							}
						}

						if (duplicateSkin)
						{
							if (gameInstance->FinalCharacters.Num() > 0)
								playerState->Character = gameInstance->FinalCharacters[playerState->Player_ID];
							if (gameInstance->FinalColours.Num() > 0)
								playerState->CharacterColour = gameInstance->FinalColours[playerState->Player_ID];
						}
					}

					if (PlayerMaterials.Num() > (int)playerState->Character * 4 + (int)playerState->CharacterColour
						&& PlayerMeshes.Num() > (int)playerState->Character)
					{
						character->PlayerMat = PlayerMaterials[(int)playerState->Character * 4 + (int)playerState->CharacterColour];
						character->PlayerMesh = PlayerMeshes[(int)playerState->Character];
					}

					UE_LOG(LogTemp, Warning, TEXT("Player %s Character: %s Colour: %s"), *FString::FromInt(playerState->Player_ID),
						*FString::FromInt((int)playerState->Character),
						*FString::FromInt((int)playerState->CharacterColour));
					UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
					//character->Server_SetCharacterMesh();

					
				}
			}
		}
	}
}

void APrototype2GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (auto gamestate = GetGameState<APrototype2Gamestate>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Gamestate"));
		if (auto playerState = Exiting->GetPlayerState<APrototype2PlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Gamemode: Removing Player %s from Server_Players"), *FString::FromInt(playerState->Player_ID));
			gamestate->Server_Players.Remove(playerState);
		}
	}
}

void APrototype2GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	KeepPlayersAtSpawnPositionUntilStart();
	
	LookOutForGameEnd();
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
		
		if (GameStateRef->HasGameFinished && !TpHasHappened)
		{
			TeleportEveryoneToPodium();
			TpHasHappened = true;
		}
	}
}

void APrototype2GameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2GameMode, EndGamePodium);
	DOREPLIFETIME(APrototype2GameMode, SellBinRef);
}

void APrototype2GameMode::TeleportEveryoneToPodium()
{
	APrototype2Character* player1{nullptr};
	APrototype2Character* player2{nullptr};
	APrototype2Character* player3{nullptr};
	APrototype2Character* player4{nullptr};

	bool bP1win{false};
	bool bP2win{false};
	bool bP3win{false};
	bool bP4win{false};
	
	int highestCoins{};

	for(int i = 0; i < Server_PlayerStates.Num(); i++)
	{
		if (auto* character = Cast<APrototype2Character>(Server_Characters[i]))
		{
			if (Server_PlayerStates[i].Get()->Coins > highestCoins)
			{
				highestCoins = Server_PlayerStates[i].Get()->Coins;
			}

			if (i == 0)
			{
				player1 = character;
			}
			if (i == 1)
			{
				player2 = character;
			}
			if (i == 2)
			{
				player3 = character;
			}
			if (i == 3)
			{
				player4 = character;
			}
		}
	}

	for(int i = 0; i < Server_PlayerStates.Num(); i++)
	{
		if (auto* character = Cast<APrototype2Character>(Server_Characters[i]))
		{
			if (Server_PlayerStates[i].Get()->Coins == highestCoins)
			{
				if (i == 0)
				{
					bP1win = true;
				}
				if (i == 1)
				{
					bP2win = true;
				}
				if (i == 2)
				{
					bP3win = true;
				}
				if (i == 3)
				{
					bP4win = true;
				}
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
			if (bP1win == true)
			{
				//player1->AttachToComponent(endGamePodium->P1WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				player1->TeleportToLocation(endGamePodium->P1WinPosition->GetComponentLocation(), endGamePodium->P1WinPosition->GetComponentRotation());
			}
			else
			{
				player1->TeleportToLocation(endGamePodium->P1LosePosition->GetComponentLocation(), endGamePodium->P1LosePosition->GetComponentRotation());
				//player1->AttachToComponent(endGamePodium->P1LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
			

			if (Server_PlayerStates.Num() > 1)
			{
				if (bP2win == true)
				{
					player2->TeleportToLocation(endGamePodium->P2WinPosition->GetComponentLocation(), endGamePodium->P2WinPosition->GetComponentRotation());
					//player2->AttachToComponent(endGamePodium->P2WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				else
				{
					player2->TeleportToLocation(endGamePodium->P2LosePosition->GetComponentLocation(), endGamePodium->P2LosePosition->GetComponentRotation());
					//player2->AttachToComponent(endGamePodium->P2LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				
			}

			if (Server_PlayerStates.Num() > 2)
			{
				if (bP3win == true)
				{
					player3->TeleportToLocation(endGamePodium->P3WinPosition->GetComponentLocation(), endGamePodium->P3WinPosition->GetComponentRotation());
					//player3->AttachToComponent(endGamePodium->P3WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				else
				{
					player3->TeleportToLocation(endGamePodium->P3LosePosition->GetComponentLocation(), endGamePodium->P3LosePosition->GetComponentRotation());
					//player3->AttachToComponent(endGamePodium->P3LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				
			}

			if (Server_PlayerStates.Num() > 3)
			{
				if (bP4win == true)
				{
					player4->TeleportToLocation(endGamePodium->P4WinPosition->GetComponentLocation(), endGamePodium->P4WinPosition->GetComponentRotation());
					//player4->AttachToComponent(endGamePodium->P4WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				else
				{
					player4->TeleportToLocation(endGamePodium->P4LosePosition->GetComponentLocation(), endGamePodium->P4LosePosition->GetComponentRotation());
					//player4->AttachToComponent(endGamePodium->P4LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				
			}
		}
	}
}

void APrototype2GameMode::Multi_TeleportEveryoneToPodium_Implementation()
{
	
}

void APrototype2GameMode::Multi_DetachShippingBinComponents_Implementation()
{
	if (auto winterBin = Cast<ASellBin_Winter>(SellBinRef))
	{
		
		//winterBin->IceBoundary->SetWorldLocation({-104.559325,-72.190911,-13.473242});
	}
}

void APrototype2GameMode::KeepPlayersAtSpawnPositionUntilStart()
{
	if (GameStateRef)
	{
		if (!GameStateRef->GameHasStarted)
		{
			for(auto player : GameStateRef->Server_Players)
			{
				if (auto controller = player->GetPlayerController())
				{
					if (auto castedController = Cast<APrototype2PlayerController>(controller))
					{
						if (auto character = castedController->GetCharacter())
						{
							TArray<AActor*> foundRadialPlots{};
							UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARadialPlot::StaticClass(), foundRadialPlots);
							for(auto plot : foundRadialPlots)
							{
								if (auto radialPlot = Cast<ARadialPlot>(plot))
								{
									if (radialPlot->Player_ID == player->Player_ID)
									{
										auto spawnPoint = radialPlot->GetActorLocation();
										spawnPoint.Z = 90.0f;
										if (FVector::Distance(character->GetActorLocation(), spawnPoint) > 200)
										{
											character->SetActorLocation(spawnPoint);
											character->SetActorRotation({character->GetActorRotation().Pitch, radialPlot->GetActorRotation().Yaw, character->GetActorRotation().Roll});
										}
										
										break;
									}
								}
							}
						}
						
					}
				}
				
			}
		}
	}
}

