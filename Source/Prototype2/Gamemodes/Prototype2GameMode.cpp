

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

	if (!DataAssetWorldOverride)
	{
		if (AActor* DataAssetWorldOverrideActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADataAssetWorldOverride::StaticClass()))
		{
			DataAssetWorldOverride = Cast<ADataAssetWorldOverride>(DataAssetWorldOverrideActor);
		}
	}

	if (auto Gamestate = GetGameState<APrototype2Gamestate>())
	{
		auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
		GameStateRef = Gamestate;
		Gamestate->SetFinalConnectionCount(GameInstance->FinalConnectionCount);
		Gamestate->TeamOneName = GameInstance->TeamOneName;
		Gamestate->TeamTwoName = GameInstance->TeamTwoName;
		Gamestate->bTeams = GameInstance->bTeams;
		bTeams = GameInstance->bTeams;
		Gamestate->TeamOneColour = GameInstance->FinalTeamAColour;
		Gamestate->TeamTwoColour = GameInstance->FinalTeamBColour;
		Gamestate->FinalTeamACount = GameInstance->FinalTeamACount;
		Gamestate->FinalTeamBCount = GameInstance->FinalTeamBCount;

		// Teams functionality
		if (GameInstance->bTeams)
		{
			SpawnTeamsPreGameArena();
			ColourTeamsPreGameArenas();
		}
		else
		{
			SpawnDefaultPregameArena();
		}

		TeleportHostToPreGameArena();
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
	
	UPrototypeGameInstance* GameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!GameInstance)
		return;

	PlayersTpdToPreGameArena_Teams.Add(false);
	
	GameStateReference->Server_Players.Add(PlayerStateReference);
	GameStateReference->SetMaxPlayersOnServer(GameInstance->MaxPlayersOnServer);
	GameStateReference->SetFinalConnectionCount(GameInstance->FinalConnectionCount);
	
	UpdatePlayerInfo(GameStateReference, GameInstance, PlayerStateReference);
	
	APrototype2Character* Character = Cast<APrototype2Character>(_NewPlayer->GetCharacter());
	if (!Character)
		return;

	UE_LOG(LogTemp, Warning, TEXT("%s Joined The Game!"), *FString(PlayerStateReference->GetPlayerName()));
\
	FVector SpawnOffset{0.0f, 0.0f , 300.0f};
	
	if (GameInstance->bTeams && PreGameArenas.Num() > 0)
	{
		if (PlayerStateReference->Details.Colour == GameInstance->FinalTeamAColour)
		{
			GameStateReference->Server_TeamOne.Add(PlayerStateReference);
			if (PreGameArenas.Num() > 0)
			{
				Character->SetActorLocation(PreGameArenas[0]->GetActorLocation() + SpawnOffset, false, nullptr, ETeleportType::ResetPhysics);
				UE_LOG(LogTemp, Warning, TEXT("Player %s Teleported To Arena A"), *FString::FromInt(GameStateReference->Server_Players.Num()));
			}
		}
		else if (PlayerStateReference->Details.Colour == GameInstance->FinalTeamBColour)
		{
			GameStateReference->Server_TeamTwo.Add(PlayerStateReference);
			if (PreGameArenas.Num() > 0)
			{
				Character->SetActorLocation(PreGameArenas[1]->GetActorLocation() + SpawnOffset, false, nullptr, ETeleportType::ResetPhysics);
				UE_LOG(LogTemp, Warning, TEXT("Player %s Teleported To Arena B"), *FString::FromInt(GameStateReference->Server_Players.Num()));
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Player %s Is On Team %s"), *FString::FromInt(GameStateReference->Server_Players.Num()), *FString::FromInt((int)PlayerStateReference->Details.Colour));
	}
	else
	{
		if (DefaultPreGameArena)
		{
			Character->SetActorLocation(DefaultPreGameArena->GetActorLocation() + SpawnOffset, false, nullptr, ETeleportType::ResetPhysics);
			UE_LOG(LogTemp, Warning, TEXT("Player %s Teleported To Arena (Solo)"), *FString::FromInt(GameStateReference->Server_Players.Num()));
		}
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

	// Default
	Prototype2Gamestate->Server_Players.Remove(PlayerStateReference);

	// Local Details
	if (PrototypeGameInstance->FinalPlayerDetails.Contains(PlayerStateReference->PlayerName))
	{
		PrototypeGameInstance->FinalPlayerDetails.Remove(PlayerStateReference->PlayerName);
	}

	// Teams
	if (Prototype2Gamestate->Server_TeamOne.Contains(PlayerStateReference))
	{
		Prototype2Gamestate->Server_TeamOne.Remove(PlayerStateReference);
	}
	if (Prototype2Gamestate->Server_TeamTwo.Contains(PlayerStateReference))
	{
		Prototype2Gamestate->Server_TeamTwo.Remove(PlayerStateReference);
	}
	
	UpdateAllPlayerInfo(Prototype2Gamestate, PrototypeGameInstance);
}

void APrototype2GameMode::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	if (!EndGamePodium && DataAssetWorldOverride->WorldOverrideData)
	{
		AActor* NewActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEndGamePodium::StaticClass());
		if (NewActor)
		{
			EndGamePodium = Cast<AEndGamePodium>(NewActor);

			if (GetGameInstance<UPrototypeGameInstance>()->bTeams)
			{
				const FTransform EndGamePodiumTransform = EndGamePodium->GetTransform();
				
				EndGamePodium->Destroy();
				EndGamePodium = nullptr;
				EndGamePodium = GetWorld()->SpawnActor<AEndGamePodium>(DataAssetWorldOverride->WorldOverrideData->EndGamePodium_Teams, EndGamePodiumTransform);
			}
		}
	}
	
	if (IsValid(SellBinRef) == false)
	{
		AActor* NewActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASellBin::StaticClass());
		if (NewActor)
		{
			SellBinRef = Cast<ASellBin>(NewActor);
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

void APrototype2GameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void APrototype2GameMode::UpdatePlayerInfo(APrototype2Gamestate* _GameStateReference,
	UPrototypeGameInstance* _gameInstanceReference, APrototype2PlayerState* _PlayerState)
{
	FString SomePlayerName{"UNASSIGNED"};
	if (auto SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM))
	{
		IOnlineIdentityPtr IdentityInterface = SteamSubsystem->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			auto UserId = _PlayerState->GetUniqueId().GetUniqueNetId();
			if (UserId.IsValid())
			{
				SomePlayerName = _PlayerState->GetPlayerName();
					
				if (_gameInstanceReference->FinalPlayerDetails.Contains(UserId->ToString()))
				{
					_PlayerState->Details = _gameInstanceReference->FinalPlayerDetails[UserId->ToString()];
				}
			}
		}
	}
	else if (auto NullSubsystem = IOnlineSubsystem::Get())
	{
		IOnlineIdentityPtr IdentityInterface = NullSubsystem->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			auto UserId = _PlayerState->GetUniqueId().GetUniqueNetId();
			if (UserId.IsValid())
			{
				SomePlayerName = "Player " + FString::FromInt(_GameStateReference->Server_Players.Num());
					
				if (_gameInstanceReference->FinalPlayerDetails.Contains(UserId->ToString()))
				{
					_PlayerState->Details = _gameInstanceReference->FinalPlayerDetails[UserId->ToString()];
				}
			}
		}
	}
		
	_PlayerState->Player_ID = _GameStateReference->Server_Players.Num();
	_PlayerState->PlayerName = SomePlayerName;

	/* Limit player name if too long and adding ... to the end */
	if (_PlayerState->PlayerName.Len() >= 10)
	{
		_PlayerState->PlayerName = _PlayerState->PlayerName.Left(FMath::Min(_PlayerState->PlayerName.Len(), 8));
		_PlayerState->PlayerName = _PlayerState->PlayerName + "...";
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
	if (PreGameArenas.Num() > 0)
		return;
	
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
}

void APrototype2GameMode::SpawnDefaultPregameArena()
{
	if (DefaultPreGameArena)
		return;
	
	DefaultPreGameArena = GetWorld()->SpawnActor<APreGameArena>(PreGameArenaPrefab, DataAssetWorldOverride->PreGameArenaSpawnLocations[0]->GetComponentTransform());
	DefaultPreGameArena->SetReplicates(true);
	DefaultPreGameArena->SetReplicateMovement(true);
	DefaultPreGameArena->SetActorTransform(DataAssetWorldOverride->PreGameArenaSpawnLocations[0]->GetComponentTransform(), false, nullptr, ETeleportType::ResetPhysics);
}

void APrototype2GameMode::TeleportHostToPreGameArena()
{
	if (!GameStateRef)
		return;

	for(auto Player : GameStateRef->Server_Players)
	{
		APrototype2Character* Character = Cast<APrototype2Character>(Player->GetPlayerController()->GetCharacter());
		if (!Character)
			return;
		
		FVector SpawnOffset{0.0f,0.0f , 300.0f};
	
		if (PreGameArenas.Num() > 1)
		{
			if (Player->Details.Colour == GameStateRef->TeamOneColour)
			{
				if (GameStateRef->Server_TeamOne.Num() <= 0)
					GameStateRef->Server_TeamOne.Add(GameStateRef->Server_Players[0]);
				Character->SetActorLocation(PreGameArenas[0]->GetActorLocation() + SpawnOffset, false, nullptr, ETeleportType::ResetPhysics);
			}
			else
			{
				if (GameStateRef->Server_TeamTwo.Num() <= 0)
					GameStateRef->Server_TeamTwo.Add(GameStateRef->Server_Players[0]);
				Character->SetActorLocation(PreGameArenas[1]->GetActorLocation() + SpawnOffset, false, nullptr, ETeleportType::ResetPhysics);
			}
		}
		else if (DefaultPreGameArena)
		{
			Character->SetActorLocation(DefaultPreGameArena->GetActorLocation() + SpawnOffset, false, nullptr, ETeleportType::ResetPhysics);
		}
	}
	
}

void APrototype2GameMode::ColourTeamsPreGameArenas()
{
	PreGameArenas[0]->AssignedTeam = 0;
	PreGameArenas[1]->AssignedTeam = 1;
}

