

#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"


ALobbyGamemode::ALobbyGamemode()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Player"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bUseSeamlessTravel = false;

}

void ALobbyGamemode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);

	if (!HasAuthority())
		return;

	auto PlayerStateReference = _NewPlayer->GetPlayerState<ALobbyPlayerState>();
	if (!PlayerStateReference)
		return;
	
	ALobbyGamestate* GameStateReference = GetGameState<ALobbyGamestate>();
	if (!GameStateReference)
		return;
	
	auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!GameInstance)
		return;
		
	GameStateReference->Server_Players.Add(PlayerStateReference);
	GameStateReference->SetMaxPlayersOnServer(GameInstance->MaxPlayersOnServer);
	
			
	ALobbyCharacter* Character = Cast<ALobbyCharacter>(_NewPlayer->GetCharacter());
	if (!Character)
		return;
	
	UpdateAllPlayerInfo(GameStateReference, GameInstance);
}

void ALobbyGamemode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);
	
	if (!HasAuthority())
		return;

	ALobbyPlayerState* PlayerStateReference = _Exiting->GetPlayerState<ALobbyPlayerState>();
	if (!PlayerStateReference)
		return;
	
	ALobbyGamestate* LobbyGamestate = GetGameState<ALobbyGamestate>();
	if (!LobbyGamestate)
		return;

	UPrototypeGameInstance* PrototypeGameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!PrototypeGameInstance)
		return;

	LobbyGamestate->Server_Players.Remove(PlayerStateReference);
	
	if (PrototypeGameInstance->FinalPlayerDetails.Contains(PlayerStateReference->PlayerName))
	{
		PrototypeGameInstance->FinalPlayerDetails.Remove(PlayerStateReference->PlayerName);
	}
	
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
		int32 SomePlayerID = i;
		FString SomePlayerName{};
		
		if (auto SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM))
		{
			IOnlineIdentityPtr IdentityInterface = SteamSubsystem->GetIdentityInterface();
			if (IdentityInterface.IsValid())
			{
				auto UserId = _GameStateReference->Server_Players[i]->GetUniqueId().GetUniqueNetId();
				if (UserId.IsValid())
				{
					SomePlayerName = _GameStateReference->Server_Players[i]->GetPlayerName();
					_gameInstanceReference->FinalPlayerDetails.FindOrAdd(UserId->ToString());
				}
			}
		}
		else if (auto NullSubsystem = IOnlineSubsystem::Get())
		{
			IOnlineIdentityPtr IdentityInterface = NullSubsystem->GetIdentityInterface();
			if (IdentityInterface.IsValid())
			{
				auto UserId = _GameStateReference->Server_Players[i]->GetUniqueId().GetUniqueNetId();
				if (UserId.IsValid())
				{
					SomePlayerName = "Player " + FString::FromInt(SomePlayerID + 1);
					_gameInstanceReference->FinalPlayerDetails.FindOrAdd(UserId->ToString());
				}
			}
		}
		
		if (ALobbyPlayerState* CasterPlayerState = Cast<ALobbyPlayerState>(_GameStateReference->Server_Players[i]))
		{
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
}


