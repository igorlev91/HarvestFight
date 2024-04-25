

#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"


ALobbyGamemode::ALobbyGamemode()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_LobbyCharacter"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bUseSeamlessTravel = false;
	bStartPlayersAsSpectators = false;
	bPauseable = false;
}

void ALobbyGamemode::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance<UPrototypeGameInstance>()->FinalPlayerDetails.Empty();
}

void ALobbyGamemode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);

	UpdateSessionJoinability();

	ALobbyPlayerState* PlayerStateReference = _NewPlayer->GetPlayerState<ALobbyPlayerState>();
	if (!IsValid(PlayerStateReference))
		return;
	
	ALobbyGamestate* GameStateReference = GetGameState<ALobbyGamestate>();
	if (!IsValid(GameStateReference))
		return;
	
	UPrototypeGameInstance* GameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!IsValid(GameInstance))
		return;

	UE_LOG(LogTemp, Warning, TEXT("%s Joined The Game!"), *FString(PlayerStateReference->GetPlayerName()));

	GameStateReference->SetMaxPlayersOnServer(GameInstance->MaxPlayersOnServer);
	
	if /* Teams */ (bTeams || (GameStateReference->Server_Players.Num() <= 0 && GameInstance->bTeams))
	{
		if (GameStateReference->TeamsDetails.Server_TeamOne.Num() <= GameStateReference->TeamsDetails.Server_TeamTwo.Num())
		{
			PlayerStateReference->Details = CreateDetailsFromColourEnum(GameStateReference->TeamsDetails.TeamOneColour);
			GameStateReference->TeamsDetails.Server_TeamOne.Add(PlayerStateReference);
		}
		else
		{
			PlayerStateReference->Details = CreateDetailsFromColourEnum(GameStateReference->TeamsDetails.TeamTwoColour);
			GameStateReference->TeamsDetails.Server_TeamTwo.Add(PlayerStateReference);
		}
		
		
		UE_LOG(LogTemp, Warning, TEXT("%s Joined Team %s"), *PlayerStateReference->GetPlayerName(), *FString::FromInt((int)PlayerStateReference->Details.Colour));
	}
	else /* NOT Teams */
	{
		if /* RE-JOINING */ (GameInstance->FinalPlayerDetails.Contains(PlayerStateReference->GetPlayerName()))
		{
			PlayerStateReference->Details = GameInstance->FinalPlayerDetails[PlayerStateReference->GetPlayerName()];
			UE_LOG(LogTemp, Warning, TEXT("%s Details Found. Color: %s"), *PlayerStateReference->GetPlayerName(), *FString::FromInt((int16)PlayerStateReference->Details.Colour));
		}
		else /* NEW PLAYER */
		{
			PlayerStateReference->Details = CreateDetailsFromColourEnum(GetFirstFreeColor(PlayerStateReference));
		}
	}

	GameStateReference->Server_Players.Add(PlayerStateReference);
	
	UpdateAllPlayerInfo(GameStateReference, GameInstance);
}

void ALobbyGamemode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);

	ALobbyPlayerState* PlayerStateReference = _Exiting->GetPlayerState<ALobbyPlayerState>();
	if (!PlayerStateReference)
		return;
	
	ALobbyGamestate* LobbyGamestate = GetGameState<ALobbyGamestate>();
	if (!LobbyGamestate)
		return;

	UPrototypeGameInstance* PrototypeGameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!PrototypeGameInstance)
		return;
		
	UE_LOG(LogTemp, Warning, TEXT("%s Attempted To Disconnect"), *PlayerStateReference->GetPlayerName());

	LobbyGamestate->Server_Players.Remove(PlayerStateReference);
	LobbyGamestate->TeamsDetails.Server_TeamOne.Remove(PlayerStateReference);
	LobbyGamestate->TeamsDetails.Server_TeamTwo.Remove(PlayerStateReference);
	
	//if (PrototypeGameInstance->FinalPlayerDetails.Find(PlayerStateReference->GetPlayerName()))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s Disconnected From The Game!"), *PlayerStateReference->GetPlayerName());
	//	PrototypeGameInstance->FinalPlayerDetails.Remove(PlayerStateReference->GetPlayerName());
	//}
	
	UpdateAllPlayerInfo(LobbyGamestate, PrototypeGameInstance);
}

void ALobbyGamemode::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
}

void ALobbyGamemode::UpdateAllPlayerInfo(ALobbyGamestate* _GameStateReference, UPrototypeGameInstance* _gameInstanceReference)
{
	for(int32 i = 0; i < _GameStateReference->Server_Players.Num(); i++)
	{
		if (!IsValid(_GameStateReference->Server_Players[i]))
			continue;
		
		int32 SomePlayerID = i;
		FString SomePlayerName{"UNASSIGNED"};

		if (auto SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM))
		{
			SomePlayerName = _GameStateReference->Server_Players[i]->GetPlayerName();
					
			if (_gameInstanceReference->FinalPlayerDetails.Contains(SomePlayerName))
			{
				_GameStateReference->Server_Players[i]->Details = _gameInstanceReference->FinalPlayerDetails[SomePlayerName];
			}
		}
		else if (auto NullSubsystem = IOnlineSubsystem::Get())
		{
			SomePlayerName = "Player " + FString::FromInt(i + 1);
					
			if (_gameInstanceReference->FinalPlayerDetails.Contains(_GameStateReference->Server_Players[i]->GetPlayerName()))
			{
				_GameStateReference->Server_Players[i]->Details = _gameInstanceReference->FinalPlayerDetails[_GameStateReference->Server_Players[i]->GetPlayerName()];
			}
		}
		
		_GameStateReference->Server_Players[i]->Player_ID = SomePlayerID;
		_GameStateReference->Server_Players[i]->PlayerName = SomePlayerName;

		/* Limit player name if too long and adding ... to the end */
		if (_GameStateReference->Server_Players[i]->PlayerName.Len() >= 10)
		{
			_GameStateReference->Server_Players[i]->PlayerName = _GameStateReference->Server_Players[i]->PlayerName.Left(FMath::Min(_GameStateReference->Server_Players[i]->PlayerName.Len(), 8));
			_GameStateReference->Server_Players[i]->PlayerName = _GameStateReference->Server_Players[i]->PlayerName + "...";
		}
	}
}



