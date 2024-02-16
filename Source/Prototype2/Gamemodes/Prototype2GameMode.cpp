

#include "Prototype2GameMode.h"

#include "NavigationSystem.h"
#include "Prototype2/Gameplay/Endgame/EndGamePodium.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gameplay/RadialPlot.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/Gameplay/PreGameArena.h"
#include "Prototype2/DataAssets/DataAssetWorldOverride.h"
#include "Prototype2/DataAssets/WorldOverrideData.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gameplay/Crown.h"
#include "Prototype2/Gameplay/RandomEventManager.h"
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
	bUseSeamlessTravel = false;

	RandomEventManager = CreateDefaultSubobject<URandomEventManager>(TEXT("Random Event Manager"));
	RandomEventManager->GamemodeOwner = this;
}

void APrototype2GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (auto Gamestate = GetGameState<APrototype2Gamestate>())
	{
		auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
		GameStateRef = Gamestate;
		Gamestate->SetFinalConnectionCount(GameInstance->FinalConnectionCount);
		Gamestate->TeamOneName = GameInstance->TeamOneName;
		Gamestate->TeamTwoName = GameInstance->TeamTwoName;
	}


}

void APrototype2GameMode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);
	
	APrototype2PlayerState* PlayerStateReference = _NewPlayer->GetPlayerState<APrototype2PlayerState>();
	if (!PlayerStateReference)
		return;
	
	APrototype2Gamestate* GameStateReference = GetGameState<APrototype2Gamestate>();
	if (!GameStateReference)
		return;
	
	auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!GameInstance)
		return;
		
	GameStateReference->Server_Players.Add(PlayerStateReference);
	GameStateReference->SetMaxPlayersOnServer(GameInstance->MaxPlayersOnServer);
	GameStateReference->SetFinalConnectionCount(GameInstance->FinalConnectionCount);

	UE_LOG(LogTemp, Warning, TEXT("%s Joined The Game!"), *FString(PlayerStateReference->GetPlayerName()));
			
	APrototype2Character* Character = Cast<APrototype2Character>(_NewPlayer->GetCharacter());
	if (!Character)
		return;
			
	Character->PlayerStateRef = PlayerStateReference;
	Character->SetPlayerState(PlayerStateReference);
	
	UpdateAllPlayerInfo(GameStateReference, GameInstance);
	TeleportToPreGameArena(Character);
	if (GameStateReference->Server_Players.Num() == 1)
	{
			
		GameStateReference->Server_TeamOne.Add(PlayerStateReference);
		return;
	}
	if (bTeams)
	{
		TArray<EColours> ScannedColours{};
		for(auto PlayerState : GameStateReference->Server_Players)
		{
			ScannedColours.Add(PlayerState->Details.Colour);
		}
		if (ScannedColours.Num() >= 2)
		{
			GameStateReference->TeamOneColour = ScannedColours[0];
			GameStateReference->TeamTwoColour = ScannedColours[1];
		}
		
		if (PlayerStateReference->Details.Colour == ScannedColours[0])
		{
			GameStateReference->Server_TeamOne.Add(PlayerStateReference);
			return;
		}
		else if (PlayerStateReference->Details.Colour == ScannedColours[1])
		{
			GameStateReference->Server_TeamTwo.Add(PlayerStateReference);
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Player %s Is On Team %s"), *FString::FromInt(GameStateReference->Server_Players.Num()), *FString::FromInt((int)PlayerStateReference->Details.Colour));
	}
}

void APrototype2GameMode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);
	
	APrototype2PlayerState* PlayerStateReference = _Exiting->GetPlayerState<APrototype2PlayerState>();
	if (!PlayerStateReference)
		return;
	
	APrototype2Gamestate* Prototype2Gamestate = GetGameState<APrototype2Gamestate>();
	if (!Prototype2Gamestate)
		return;

	UPrototypeGameInstance* PrototypeGameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!PrototypeGameInstance)
		return;

	Prototype2Gamestate->Server_Players.Remove(PlayerStateReference);
	Prototype2Gamestate->Server_TeamOne.Remove(PlayerStateReference);
	Prototype2Gamestate->Server_TeamTwo.Remove(PlayerStateReference);
	if (PrototypeGameInstance->FinalPlayerDetails.Contains(PlayerStateReference->PlayerName))
	{
		PrototypeGameInstance->FinalPlayerDetails.Remove(PlayerStateReference->PlayerName);
	}
	
	UpdateAllPlayerInfo(Prototype2Gamestate, PrototypeGameInstance);
}

void APrototype2GameMode::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	if (!EndGamePodium)
	{
		AActor* NewActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEndGamePodium::StaticClass());
		if (NewActor)
		{
			EndGamePodium = Cast<AEndGamePodium>(NewActor);
		}
	}
	
	if (!SellBinRef)
	{
		AActor* NewActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASellBin::StaticClass());
		if (NewActor)
		{
			SellBinRef = Cast<ASellBin>(NewActor);
		}
	}
	
	if (!DataAssetWorldOverride)
	{
		if (AActor* DataAssetWorldOverrideActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADataAssetWorldOverride::StaticClass()))
		{
			DataAssetWorldOverride = Cast<ADataAssetWorldOverride>(DataAssetWorldOverrideActor);
			

			SpawnDefaultPregameArena();
		}
	}

	if (DataAssetWorldOverride->WorldOverrideData && !KingCrown)
	{
		if (DataAssetWorldOverride->WorldOverrideData->KingCrown)
			KingCrown = GetWorld()->SpawnActor<ACrown>(DataAssetWorldOverride->WorldOverrideData->KingCrown);
		else
			KingCrown = GetWorld()->SpawnActor<ACrown>(ACrown::StaticClass());
	}

	if (KingCrown)
	{
		if (AutomaticCrownCheckTimer <= 0)
		{
			AutomaticCrownCheckTimer = AutomaticCrownCheckFrequency;
		
		}
		else
		{
			AutomaticCrownCheckTimer -= _DeltaSeconds;
		}

		if (bTeams)
		{
			KingCrown->Destroy();
			KingCrown = nullptr;
		}
	}

	// Teams functionality
	SpawnTeamsPreGameArena();
	TeleportHostToPreGameArena();
	TeleportHostToTeamsPreGameArena();
	ColourTeamsPreGameArenas();
	TeleportUnteleportedPlayersToPreGameArena_Teams();
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
			//TeleportEveryoneToPodium();
			//PupeteerPlayerCharactersForEndGame();
			bTpHasHappened = true;
		}
	}
}

void APrototype2GameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void APrototype2GameMode::TeleportEveryoneToPodium()
{
	bool bP1win{false};
	bool bP2win{false};
	bool bP3win{false};
	bool bP4win{false};
	
	int32 HighestCoins{};

	for(auto i = 0; i < Server_PlayerStates.Num(); i++)
	{
		if (Server_PlayerStates[i].Get()->Coins > HighestCoins)
		{
			HighestCoins = Server_PlayerStates[i].Get()->Coins;
		}
	}

	for(auto i = 0; i < Server_PlayerStates.Num(); i++)
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
	if (!EndGamePodium)
	{
		return;
	}
		
	FTransform defautTransform{};
	defautTransform.SetScale3D({1.0f, 1.0f, 1.0f});
		
	APrototype2Character* CurrentCharacter{nullptr};
	
	// Tp Everyone
	if (Server_Characters.Num() > 0 && EndGamePodium)
	{
		CurrentCharacter = Server_Characters[0].Get();
		if (!CurrentCharacter)
			return;
			
		if (bP1win == true)
		{
			//player1->AttachToComponent(endGamePodium->P1WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetWinPosition(0)->GetComponentLocation(), EndGamePodium->GetWinPosition(0)->GetComponentRotation());
		}
		else
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetLossPosition(0)->GetComponentLocation(), EndGamePodium->GetLossPosition(0)->GetComponentRotation());
			//player1->AttachToComponent(endGamePodium->P1LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
		
	if (Server_Characters.Num() > 1 && EndGamePodium)
	{
		CurrentCharacter = Server_Characters[1].Get();
		if (!CurrentCharacter)
			return;
		
		if (bP2win == true)
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetWinPosition(1)->GetComponentLocation(), EndGamePodium->GetWinPosition(1)->GetComponentRotation());
			//player2->AttachToComponent(endGamePodium->P2WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetLossPosition(1)->GetComponentLocation(), EndGamePodium->GetLossPosition(1)->GetComponentRotation());
			//player2->AttachToComponent(endGamePodium->P2LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		
	}
	if (Server_Characters.Num() > 2 && EndGamePodium)
	{
		CurrentCharacter = Server_Characters[2].Get();
		if (!CurrentCharacter)
			return;
		if (bP3win == true)
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetWinPosition(2)->GetComponentLocation(), EndGamePodium->GetWinPosition(2)->GetComponentRotation());
			//player3->AttachToComponent(endGamePodium->P3WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetLossPosition(2)->GetComponentLocation(), EndGamePodium->GetLossPosition(2)->GetComponentRotation());
			//player3->AttachToComponent(endGamePodium->P3LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		
	}
	if (Server_Characters.Num() > 3 && EndGamePodium)
	{
		CurrentCharacter = Server_Characters[3].Get();
		if (!CurrentCharacter)
			return;
		if (bP4win == true)
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetWinPosition(3)->GetComponentLocation(), EndGamePodium->GetWinPosition(3)->GetComponentRotation());
			//player4->AttachToComponent(endGamePodium->P4WinPosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			CurrentCharacter->TeleportToLocation(EndGamePodium->GetLossPosition(3)->GetComponentLocation(), EndGamePodium->GetLossPosition(3)->GetComponentRotation());
			//player4->AttachToComponent(endGamePodium->P4LosePosition,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

void APrototype2GameMode::Multi_AssignCharacterSkin_Implementation(APrototype2PlayerState* _CastedPlayerState, APrototype2Gamestate* _GameStateReference, UPrototypeGameInstance* _gameInstanceReference, const FString& _NewName)
{
	APlayerController* SomeController = _CastedPlayerState->GetPlayerController();
	if (!SomeController)
		return;

	ACharacter* SomeCharacter = SomeController->GetCharacter();
	if (!SomeCharacter)
		return;

	APrototype2Character* SomeCastedCharacter = Cast<APrototype2Character>(SomeCharacter);
	if (!SomeCastedCharacter)
		return;
			
	int32 CharacterSelection = (int32)_gameInstanceReference->FinalPlayerDetails[_NewName].Character;

	SomeCastedCharacter->GetMesh()->SetSkeletalMesh(AnimationDatas[CharacterSelection]->SkeletalMesh);
	SomeCastedCharacter->GetMesh()->SetMaterial(0, PlayerMaterials[CharacterSelection]);
	if (auto MaterialInstance = SomeCastedCharacter->GetMesh()->CreateDynamicMaterialInstance(0))
	{
		MaterialInstance->SetVectorParameterValue(FName("Cow Colour"), _gameInstanceReference->FinalPlayerDetails[_NewName].CharacterColour);
		MaterialInstance->SetVectorParameterValue(FName("Spot Colour"), _gameInstanceReference->FinalPlayerDetails[_NewName].CharacterSubColour);
		SomeCastedCharacter->GetMesh()->SetMaterial(0, MaterialInstance);
	}
}

void APrototype2GameMode::Multi_TeleportEveryoneToPodium_Implementation()
{
	
}

void APrototype2GameMode::KeepPlayersAtSpawnPositionUntilStart()
{
	if (GameStateRef)
	{
		if (!GameStateRef->HasGameStarted())
		{
			for(auto Player : GameStateRef->Server_Players)
			{
				if (APlayerController* Controller = Player->GetPlayerController())
				{
					if (ACharacter* Character = Controller->GetCharacter())
					{
						TArray<AActor*> FoundRadialPlots{};
						UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARadialPlot::StaticClass(), FoundRadialPlots);
						for(AActor* Plot : FoundRadialPlots)
						{
							if (ARadialPlot* RadialPlot = Cast<ARadialPlot>(Plot))
							{
								if (RadialPlot->GetPlayerID() == Player->Details.Colour)
								{
									FVector SpawnPoint = RadialPlot->GetActorLocation();
									SpawnPoint.Z = PlayerStartingZPosition;
									if (FVector::Distance(Character->GetActorLocation(), SpawnPoint) > 200)
									{
										Character->SetActorLocation(SpawnPoint);
										Character->SetActorRotation({Character->GetActorRotation().Pitch, RadialPlot->GetActorRotation().Yaw, Character->GetActorRotation().Roll});

										/* Move player forward towards sell bin */
										FVector ForwardVector = Character->GetActorForwardVector();
										FVector NewLocation = Character->GetActorLocation() + ForwardVector * PlayerStartingDistanceTowardsSellBin;
										Character->SetActorLocation(NewLocation);
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

void APrototype2GameMode::PupeteerPlayerCharactersForEndGame()
{
	if (GameStateRef)
	{
		if (EndGamePodium->GetEndGameCamera())
		{
			if (GameStateRef->HasGameFinished())
			{
				for(auto PlayerState : GameStateRef->PlayerArray)
				{
					if (auto ControllerCast = Cast<APrototype2PlayerController>(PlayerState->GetPlayerController()))
					{
						if (auto CharacterCast = Cast<APrototype2Character>(ControllerCast->GetCharacter()))
						{
							CharacterCast->GetCharacterMovement()->StopActiveMovement();
							CharacterCast->GetCharacterMovement()->StopMovementImmediately();
							CharacterCast->GetCharacterMovement()->DisableMovement();
							CharacterCast->GetCharacterMovement()->Velocity = {};
						}
					}
				}
			}
		}
	}
}


void APrototype2GameMode::UpdateAllPlayerInfo(APrototype2Gamestate* _GameStateReference, UPrototypeGameInstance* _gameInstanceReference)
{
	for(int32 i = 0; i < _GameStateReference->Server_Players.Num(); i++)
	{
		APrototype2PlayerState* CasterPlayerState = Cast<APrototype2PlayerState>(_GameStateReference->Server_Players[i]);
		if (!CasterPlayerState)
			continue;
		
		int32 SomePlayerID = i;
		FString SomePlayerName{"UNASSIGNED"};

		if (auto SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM))
		{
			IOnlineIdentityPtr IdentityInterface = SteamSubsystem->GetIdentityInterface();
			if (IdentityInterface.IsValid())
			{
				auto UserId = CasterPlayerState->GetUniqueId().GetUniqueNetId();
				if (UserId.IsValid())
				{
					SomePlayerName = _GameStateReference->Server_Players[i]->GetPlayerName();
					
					if (_gameInstanceReference->FinalPlayerDetails.Contains(UserId->ToString()))
					{
						CasterPlayerState->Details = _gameInstanceReference->FinalPlayerDetails[UserId->ToString()];
					}
				}
			}
		}
		else if (auto NullSubsystem = IOnlineSubsystem::Get())
		{
			IOnlineIdentityPtr IdentityInterface = NullSubsystem->GetIdentityInterface();
			if (IdentityInterface.IsValid())
			{
				auto UserId = CasterPlayerState->GetUniqueId().GetUniqueNetId();
				if (UserId.IsValid())
				{
					SomePlayerName = "Player " + FString::FromInt(SomePlayerID + 1);
					
					if (_gameInstanceReference->FinalPlayerDetails.Contains(UserId->ToString()))
					{
						CasterPlayerState->Details = _gameInstanceReference->FinalPlayerDetails[UserId->ToString()];
					}
				}
			}
		}
		
		CasterPlayerState->Player_ID = SomePlayerID;
		CasterPlayerState->PlayerName = SomePlayerName;

		/* Limit player name if too long and adding ... to the end */
		if (CasterPlayerState->PlayerName.Len() >= 10)
		{
			CasterPlayerState->PlayerName = CasterPlayerState->PlayerName.Left(FMath::Min(CasterPlayerState->PlayerName.Len(), 8));
			CasterPlayerState->PlayerName = CasterPlayerState->PlayerName + "...";
		}
	}
}

void APrototype2GameMode::SpawnTeamsPreGameArena()
{
	if (bTeams && !bPreGameArenasAdjustedForTeams && PreGameArenaPrefab)
	{
		DefaultPreGameArena->Destroy();
		
		APreGameArena* TeamAArena = GetWorld()->SpawnActor<APreGameArena>(PreGameArenaPrefab, DataAssetWorldOverride->PreGameArenaSpawnLocations[1]->GetComponentTransform());
		TeamAArena->SetReplicates(true);
		TeamAArena->SetReplicateMovement(true);
		TeamAArena->SetActorTransform(DataAssetWorldOverride->PreGameArenaSpawnLocations[1]->GetComponentTransform(), false, nullptr, ETeleportType::ResetPhysics);
		PreGameArenas.Add(TeamAArena);
	
		APreGameArena* TeamBArena = GetWorld()->SpawnActor<APreGameArena>(PreGameArenaPrefab, DataAssetWorldOverride->PreGameArenaSpawnLocations[2]->GetComponentTransform());
		TeamBArena->SetReplicates(true);
		TeamBArena->SetReplicateMovement(true);
		TeamBArena->SetActorTransform(DataAssetWorldOverride->PreGameArenaSpawnLocations[2]->GetComponentTransform(), false, nullptr, ETeleportType::ResetPhysics);
		PreGameArenas.Add(TeamBArena);

		bPreGameArenasAdjustedForTeams = true;
	}
}

void APrototype2GameMode::TeleportToPreGameArena(APrototype2Character* _Player)
{ 
	GameStateRef = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameStateRef)
		return;
	if (GameStateRef->PlayerArray.Num() <= 1)
	{
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->SetActorLocation({0.0f, 0.0f, 1100.0f});
	}
	else
	{
		if (_Player && DefaultPreGameArena)
			_Player->SetActorLocation(DefaultPreGameArena->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
	}

	PlayersTpdToPreGameArena_Teams.Add(false);
}

void APrototype2GameMode::TeleportUnteleportedPlayersToPreGameArena_Teams()
{
	if (!bTeams)
		return;
	
	if (GameStateRef->TeamOneColour == GameStateRef->TeamTwoColour)
		return;

	if (PreGameArenas.Num() < 2)
		return;
	
	for (int i = 0; i < PlayersTpdToPreGameArena_Teams.Num(); i++)
	{
		if (PlayersTpdToPreGameArena_Teams[i] == false)
		{
			APrototype2Character* _Player = Cast<APrototype2Character>(GameStateRef->Server_Players[i]->GetPlayerController()->GetCharacter());
			if (!_Player)
				continue;
			
			if (GameStateRef->Server_Players[i]->Details.Colour == GameStateRef->TeamOneColour)
			{
				_Player->SetActorLocation(PreGameArenas[0]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
				PlayersTpdToPreGameArena_Teams[i] = true;
			}
			else if (GameStateRef->Server_Players[i]->Details.Colour == GameStateRef->TeamTwoColour)
			{
				_Player->SetActorLocation(PreGameArenas[1]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
				PlayersTpdToPreGameArena_Teams[i] = true;
			}
			else
			{
				_Player->SetActorLocation(DefaultPreGameArena->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
				PlayersTpdToPreGameArena_Teams[i] = true;
			}
		}
	}
}

void APrototype2GameMode::SpawnDefaultPregameArena()
{
	DefaultPreGameArena = GetWorld()->SpawnActor<APreGameArena>(PreGameArenaPrefab, DataAssetWorldOverride->PreGameArenaSpawnLocations[0]->GetComponentTransform());
	DefaultPreGameArena->SetReplicates(true);
	DefaultPreGameArena->SetReplicateMovement(true);
	DefaultPreGameArena->SetActorTransform(DataAssetWorldOverride->PreGameArenaSpawnLocations[0]->GetComponentTransform(), false, nullptr, ETeleportType::ResetPhysics);
}

void APrototype2GameMode::TeleportHostToPreGameArena()
{
	if (!bPreGameArenasAdjustedForTeams)
		return;
	
	if (!bHostHasTpdToPreGameArena)
	{
		APrototype2Character* ServerCharacter = Cast<APrototype2Character>(GameStateRef->Server_Players[0]->GetPlayerController()->GetCharacter());
		if (PreGameArenas.Num() > 1)
		{
			if (GameStateRef->Server_Players[0]->Details.Colour == GameStateRef->TeamOneColour)
				ServerCharacter->SetActorLocation(PreGameArenas[0]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
			else
				ServerCharacter->SetActorLocation(PreGameArenas[1]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
		}
		else if (DefaultPreGameArena)
		{
			ServerCharacter->SetActorLocation(DefaultPreGameArena->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
		}

		bHostHasTpdToPreGameArena = true;
	}
}

void APrototype2GameMode::TeleportHostToTeamsPreGameArena()
{
	if (!bPreGameArenasAdjustedForTeams)
		return;
	
	if (!bHostHasTpdToPreGameArena)
		return;
	
	if (!bHostHasTpdToTeamsPreGameArena && bTeams && PreGameArenas.Num() > 1 && IsValid(PreGameArenas[1]))
	{
		APrototype2Character* ServerCharacter = Cast<APrototype2Character>(GameStateRef->Server_Players[0]->GetPlayerController()->GetCharacter());

		if (ServerCharacter)
		{
			if (GameStateRef->Server_Players[0]->Details.Colour == GameStateRef->TeamOneColour)
				ServerCharacter->SetActorLocation(PreGameArenas[0]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
			else if (GameStateRef->Server_Players[0]->Details.Colour == GameStateRef->TeamTwoColour)
				ServerCharacter->SetActorLocation(PreGameArenas[1]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);
			else
				ServerCharacter->SetActorLocation(PreGameArenas[0]->GetActorLocation() + (FVector::UpVector * 200.0f), false, nullptr, ETeleportType::ResetPhysics);

			
			bHostHasTpdToTeamsPreGameArena = true;
		}
	}
}

void APrototype2GameMode::ColourTeamsPreGameArenas()
{
	if (!bPreGameArenasAdjustedForTeams)
		return;

	if (bHasSetTeamsColours)
		return;
	
	if (GameStateRef->TeamOneColour != GameStateRef->TeamTwoColour
		&& !(GameStateRef->TeamOneColour == EColours::RED && GameStateRef->TeamTwoColour == EColours::RED))
	{
		PreGameArenas[0]->AssignedTeam = 0;
		PreGameArenas[1]->AssignedTeam = 1;
		bHasSetTeamsColours = true;
	}
}


void APrototype2GameMode::Multi_PupeteerPlayerCharactersForEndGame_Implementation(APrototype2Character* _Target)
{
	_Target->GetCharacterMovement()->StopActiveMovement();
	_Target->GetCharacterMovement()->StopMovementImmediately();
	_Target->GetCharacterMovement()->DisableMovement();
	_Target->GetCharacterMovement()->Velocity = {};
}

