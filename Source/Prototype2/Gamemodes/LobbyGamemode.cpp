

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

	
}

void ALobbyGamemode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);
	
	if (HasAuthority())
	{
		if (ALobbyPlayerState* PlayerStateReference = _NewPlayer->GetPlayerState<ALobbyPlayerState>())
		{
			if (ALobbyGamestate* GameStateReference = GetGameState<ALobbyGamestate>())
			{
				PlayerStateReference->Player_ID = GameStateReference->Server_Players.Add(PlayerStateReference);
				
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
				
				if (auto Character = Cast<ALobbyCharacter>(_NewPlayer->GetCharacter()))
				{
					Character->SetPlayerStateRef(PlayerStateReference);
					Character->SetPlayerState(PlayerStateReference);
					Character->SetOwner(_NewPlayer);
					GameStateReference->SetMaxPlayersOnServer(GetGameInstance<UPrototypeGameInstance>()->MaxPlayersOnServer);

					/* Reset Player 1 position */
					if (PlayerStateReference->Player_ID == 0)
					{
						/* Set player 1 starting position */
						if (GameStateReference->GetMaxPlayersOnServer() == 3)
						{
							Player1StartPosition.Y = Player1StartPosition.Y + (DistanceBetweenPlayers / 2.0f);
						}
						else if (GameStateReference->GetMaxPlayersOnServer() == 2)
						{
							Player1StartPosition.Y = Player1StartPosition.Y + DistanceBetweenPlayers;
						}
						else
						{
							// Nothing - player position stays where it was
						}
						UE_LOG(LogTemp, Warning, TEXT("Player 1 position changed"));
						Character->SetActorLocation(Player1StartPosition);
					}

					/* Set player 2 - 4 positions  */
					for (int32 i = 1; i < 4; i++)
					{
						if (i == PlayerStateReference->Player_ID)
						{
							FVector NewPosition = Player1StartPosition + FVector(0, DistanceBetweenPlayers * i, 0);

							Character->SetActorLocation(NewPosition);
						}
					}

					/* Set player 5 - 8 positions  */
					for (int32 i = 4; i < 8; i++)
					{
						if (i == PlayerStateReference->Player_ID)
						{
							FVector NewPosition = Player1StartPosition + FVector(200.0f, (DistanceBetweenPlayers / 2.0f) * i, 0);

							Character->SetActorLocation(NewPosition);
						}
					}
				}
			}
		}
	}
}

void ALobbyGamemode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);

	if (auto LobbyGamestate = GetGameState<ALobbyGamestate>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemode: Got Gamestate"));
		if (auto PlayerStateReference = _Exiting->GetPlayerState<ALobbyPlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Gamemode: Removing Player %s froom Server_Players"), *FString::FromInt(PlayerStateReference->Player_ID));
			LobbyGamestate->Server_Players.Remove(PlayerStateReference);
		}
	}
}

void ALobbyGamemode::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	//auto GamestateReference = GetGameState<ALobbyGamestate>();
	//if (!GamestateReference)
	//{
	//	return;
	//}
//
	//for(auto i = 0; i < GamestateReference->Server_Players.Num(); i++)
	//{
	//	auto PlayerStateReference = GamestateReference->Server_Players[i];
	//	if (!PlayerStateReference)
	//	{
	//		continue;
	//	}
	//	
	//	auto ControllerReference = PlayerStateReference->GetPlayerController();
	//	if (!ControllerReference)
	//	{
	//		continue;
	//	}
//
	//	auto CharacterReference = Cast<ALobbyCharacter>(ControllerReference->GetCharacter());
	//	if (!CharacterReference)
	//	{
	//		continue;
	//	}
	//	
	//	if (PlayerMaterials.Num() > (int32)PlayerStateReference->Character)
	//		CharacterReference->SetPlayerMat(PlayerMaterials[(int32)PlayerStateReference->Character]);
	//}
	
}


