#include "Prototype2GameMode.h"

#include "Prototype2/Gameplay/Endgame/EndGamePodium.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gameplay/RadialPlot.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"

class ALobbyGamestate;

AEndGamePodium* APrototype2GameMode::GetEndGamePodium()
{
	return EndGamePodium;
}

APrototype2GameMode::APrototype2GameMode()
{
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

	if (auto Gamestate = GetGameState<APrototype2Gamestate>())
	{
		GameStateRef = Gamestate;
		Gamestate->SetFinalConnectionCount(GetGameInstance<UPrototypeGameInstance>()->FinalConnectionCount);
	}
	if (EndGamePodiumPrefab)
	{
		EndGamePodium = GetWorld()->SpawnActor<AEndGamePodium>(EndGamePodiumPrefab,
			FVector{3031.58f,-1426.65f,-17.30},
			FRotator{0.0f,140.59f,0.0f});
		EndGamePodium->SetReplicates(true);
		EndGamePodium->SetReplicatingMovement(true);
	}
	
	if (SellBinPrefab)
	{
		SellBinRef = GetWorld()->SpawnActor<ASellBin>(SellBinPrefab,
			{-104.559325,-72.190911,-30.0f},
			FRotator::ZeroRotator);
		SellBinRef->SetReplicates(true);
		SellBinRef->SetReplicatingMovement(true);
		if (auto winterSellBin = Cast<ASellBin_Winter>(SellBinRef))
		{
			winterSellBin->SetActorLocation({-104.559325,-72.190911,-13.473242});
		}
		Multi_DetachShippingBinComponents();
	}
}

void APrototype2GameMode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);
	
	if (HasAuthority())
	{
		if (APrototype2PlayerState* PlayerStateReference = _NewPlayer->GetPlayerState<APrototype2PlayerState>())
		{
			if (APrototype2Gamestate* Gamestate = GetGameState<APrototype2Gamestate>())
			{
				PlayerStateReference->Player_ID = Gamestate->RegisterPlayer(PlayerStateReference);

				FString NewPlayerName = FString("Player ") + FString::FromInt(PlayerStateReference->Player_ID + 1);
				auto SubSystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
				if (SubSystem)
				{
					IOnlineIdentityPtr IdentityInterface = SubSystem->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						TSharedPtr<const FUniqueNetId> UserId = PlayerStateReference->GetUniqueId().GetUniqueNetId();
						if (UserId.IsValid())
						{
							if (IdentityInterface->GetLoginStatus(*UserId) == ELoginStatus::LoggedIn ||
							IdentityInterface->GetLoginStatus(*UserId) == ELoginStatus::UsingLocalProfile)
							{
								NewPlayerName = IdentityInterface->GetPlayerNickname(*UserId);
								UE_LOG(LogTemp, Warning, TEXT("Player %s Has Steam Name %s"), *FString::FromInt(PlayerStateReference->GetPlayerId()), *NewPlayerName);
							}
						}
					}
				}
				PlayerStateReference->PlayerName = NewPlayerName;
				
				if (auto Character = Cast<APrototype2Character>(_NewPlayer->GetCharacter()))
				{
					Character->PlayerStateRef = PlayerStateReference;
					Character->SetPlayerState(PlayerStateReference);
					Character->SetOwner(_NewPlayer);
					
					if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
					{
						Gamestate->SetMaxPlayersOnServer(GameInstance->MaxPlayersOnServer);
						Gamestate->SetFinalConnectionCount(GameInstance->FinalConnectionCount);

						FString Name{};
						auto OnlineSubsystem = IOnlineSubsystem::Get();
						if (OnlineSubsystem)
						{
							if (auto Interface = OnlineSubsystem->GetIdentityInterface())
							{
								if (auto uniqueID = PlayerStateReference->GetUniqueId().GetUniqueNetId())
								{
									Name = Interface->GetPlayerNickname(*uniqueID);
								}
							}
						}
						if (GameInstance->FinalPlayerDetails.Contains(Name))
						{
							PlayerStateReference->Character = GameInstance->FinalPlayerDetails[Name].Character;
							PlayerStateReference->CharacterColour = GameInstance->FinalPlayerDetails[Name].CharacterColour;
						}
					}

					if (PlayerMaterials.Num() > (int)PlayerStateReference->Character && PlayerMeshes.Num() > (int)PlayerStateReference->Character)
					{
						// Set the animation data asset for the character
						Character->AnimationData = AnimationDatas[(int)PlayerStateReference->Character];
						
						//Character->PlayerMat = UMaterialInstanceDynamic::Create(PlayerMaterials[(int)PlayerStateReference->Character], nullptr);
						Character->PlayerMesh = PlayerMeshes[(int)PlayerStateReference->Character]; // Todo: Remove when animation data set up
					}
				}
			}
		}
	}
}

void APrototype2GameMode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);

	if (auto Gamestate = GetGameState<APrototype2Gamestate>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Gamestate"));
		if (auto PlayerState = _Exiting->GetPlayerState<APrototype2PlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Gamemode: Removing Player %s from Server_Players"), *FString::FromInt(PlayerState->Player_ID));
			Gamestate->UnRegisterPlayer(PlayerState);
		}
	}
}

void APrototype2GameMode::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);

	KeepPlayersAtSpawnPositionUntilStart();
	
	LookOutForGameEnd();
}

void APrototype2GameMode::DisableControllerInput(APlayerController* _PlayerController)
{
	if (_PlayerController != nullptr)
	{
		_PlayerController->DisableInput(_PlayerController);
		_PlayerController->SetIgnoreMoveInput(true);
		_PlayerController->SetIgnoreLookInput(true);
	}
}

void APrototype2GameMode::EnableControllerInput(APlayerController* _PlayerController)
{
	if (_PlayerController != nullptr)
	{
		_PlayerController->EnableInput(_PlayerController);
		_PlayerController->SetIgnoreMoveInput(false);
		_PlayerController->SetIgnoreLookInput(false);
	}
}

void APrototype2GameMode::DisableControllerInputForAll()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
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
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
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
		if (Server_Characters.Num() < GameStateRef->GetCurrentConnectionCount())
		{
			Server_Characters.Empty();
			Server_Characters = {};
			for(auto PlayerState : GameStateRef->Server_Players)
			{
				if (auto Character = Cast<APrototype2Character>(PlayerState->GetPlayerController()->GetCharacter()))
				{
					Server_Characters.Add(Character);
				}
			}
		}

		Server_PlayerStates = GameStateRef->Server_Players;
		
		if (GameStateRef->HasGameFinished() && !bTpHasHappened)
		{
			TeleportEveryoneToPodium();
			bTpHasHappened = true;
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
	APrototype2Character* Player1{nullptr};
	APrototype2Character* Player2{nullptr};
	APrototype2Character* Player3{nullptr};
	APrototype2Character* Player4{nullptr};

	bool bP1win{false};
	bool bP2win{false};
	bool bP3win{false};
	bool bP4win{false};
	
	int32 HighestCoins{};

	for(auto i = 0; i < Server_PlayerStates.Num(); i++)
	{
		if (auto Character = Cast<APrototype2Character>(Server_Characters[i]))
		{
			if (Server_PlayerStates[i].Get()->Coins > HighestCoins)
			{
				HighestCoins = Server_PlayerStates[i].Get()->Coins;
			}

			if (i == 0)
			{
				Player1 = Character;
			}
			if (i == 1)
			{
				Player2 = Character;
			}
			if (i == 2)
			{
				Player3 = Character;
			}
			if (i == 3)
			{
				Player4 = Character;
			}
		}
	}

	for(auto i = 0; i < Server_PlayerStates.Num(); i++)
	{
		if (auto Character = Cast<APrototype2Character>(Server_Characters[i]))
		{
			if (Server_PlayerStates[i].Get()->Coins == HighestCoins)
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
				Player1->TeleportToLocation(endGamePodium->GetWinPosition(0)->GetComponentLocation(), endGamePodium->GetWinPosition(0)->GetComponentRotation());
			}
			else
			{
				Player1->TeleportToLocation(endGamePodium->GetLossPosition(0)->GetComponentLocation(), endGamePodium->GetLossPosition(0)->GetComponentRotation());
				//player1->AttachToComponent(endGamePodium->P1LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
			

			if (Server_PlayerStates.Num() > 1)
			{
				if (bP2win == true)
				{
					Player2->TeleportToLocation(endGamePodium->GetWinPosition(1)->GetComponentLocation(), endGamePodium->GetWinPosition(1)->GetComponentRotation());
					//player2->AttachToComponent(endGamePodium->P2WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				else
				{
					Player2->TeleportToLocation(endGamePodium->GetLossPosition(1)->GetComponentLocation(), endGamePodium->GetLossPosition(1)->GetComponentRotation());
					//player2->AttachToComponent(endGamePodium->P2LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				
			}

			if (Server_PlayerStates.Num() > 2)
			{
				if (bP3win == true)
				{
					Player3->TeleportToLocation(endGamePodium->GetWinPosition(2)->GetComponentLocation(), endGamePodium->GetWinPosition(2)->GetComponentRotation());
					//player3->AttachToComponent(endGamePodium->P3WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				else
				{
					Player3->TeleportToLocation(endGamePodium->GetLossPosition(2)->GetComponentLocation(), endGamePodium->GetLossPosition(2)->GetComponentRotation());
					//player3->AttachToComponent(endGamePodium->P3LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				
			}

			if (Server_PlayerStates.Num() > 3)
			{
				if (bP4win == true)
				{
					Player4->TeleportToLocation(endGamePodium->GetWinPosition(3)->GetComponentLocation(), endGamePodium->GetWinPosition(3)->GetComponentRotation());
					//player4->AttachToComponent(endGamePodium->P4WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				}
				else
				{
					Player4->TeleportToLocation(endGamePodium->GetLossPosition(3)->GetComponentLocation(), endGamePodium->GetLossPosition(3)->GetComponentRotation());
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
	if (auto WinterBin = Cast<ASellBin_Winter>(SellBinRef))
	{
		
		//winterBin->IceBoundary->SetWorldLocation({-104.559325,-72.190911,-13.473242});
	}
}

void APrototype2GameMode::KeepPlayersAtSpawnPositionUntilStart()
{
	if (GameStateRef)
	{
		if (!GameStateRef->HasGameStarted())
		{
			for(auto Player : GameStateRef->Server_Players)
			{
				if (auto Controller = Player->GetPlayerController())
				{
					if (auto CastedController = Cast<APrototype2PlayerController>(Controller))
					{
						if (auto Character = CastedController->GetCharacter())
						{
							TArray<AActor*> FoundRadialPlots{};
							UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARadialPlot::StaticClass(), FoundRadialPlots);
							for(auto Plot : FoundRadialPlots)
							{
								if (auto RadialPlot = Cast<ARadialPlot>(Plot))
								{
									if (RadialPlot->GetPlayerID() == Player->Player_ID)
									{
										auto SpawnPoint = RadialPlot->GetActorLocation();
										SpawnPoint.Z = 90.0f;
										if (FVector::Distance(Character->GetActorLocation(), SpawnPoint) > 200)
										{
											Character->SetActorLocation(SpawnPoint);
											Character->SetActorRotation({Character->GetActorRotation().Pitch, RadialPlot->GetActorRotation().Yaw, Character->GetActorRotation().Roll});
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

