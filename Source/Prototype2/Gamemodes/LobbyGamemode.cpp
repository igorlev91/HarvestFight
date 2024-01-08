

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
				PlayerStateReference->CharacterColour = (ECharacterColours)((rand() % 3) + 1);
				int32 NumberOfPlayersWithSameColour{2};
				while(NumberOfPlayersWithSameColour >= 1)
				{
					NumberOfPlayersWithSameColour = 0;
					for(auto OtherPlayerState : GameStateReference->Server_Players)
					{
						if (OtherPlayerState->CharacterColour == PlayerStateReference->CharacterColour)
						{
							NumberOfPlayersWithSameColour++;
						}
					}
					if (NumberOfPlayersWithSameColour >= 1)
					{
						int32 NewColour = (int32)PlayerStateReference->CharacterColour;
						NewColour ++;
						if (NewColour > 3)
						{
							NewColour = 0;
						}
						else if (NewColour < 0)
						{
							NewColour = 3;
						}
						PlayerStateReference->CharacterColour = (ECharacterColours)NewColour;
					}
				}
				
				//UE_LOG(LogTemp, Warning, TEXT("Player ID Assigned"));
				PlayerStateReference->Player_ID = GameStateReference->Server_Players.Add(PlayerStateReference);
				
				FString NewPlayerName = FString("Player ") + FString::FromInt(PlayerStateReference->Player_ID + 1);
				auto SubSystem = IOnlineSubsystem::Get(FName("Steam"));
				if (SubSystem)
				{
					IOnlineIdentityPtr IdentityInterface = SubSystem->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						if (IdentityInterface->GetLoginStatus(PlayerStateReference->GetPlayerId()) == ELoginStatus::LoggedIn ||
							IdentityInterface->GetLoginStatus(PlayerStateReference->GetPlayerId()) == ELoginStatus::UsingLocalProfile)
						{
							NewPlayerName = IdentityInterface->GetPlayerNickname(PlayerStateReference->GetPlayerId());
							UE_LOG(LogTemp, Warning, TEXT("Player %s Has Steam Name %s"), *FString::FromInt(PlayerStateReference->GetPlayerId()), *NewPlayerName);
						}
					}
				}
				PlayerStateReference->PlayerName = NewPlayerName;

				if (auto LobbyCharacterCast = Cast<ALobbyCharacter>(_NewPlayer->GetCharacter()))
				{
					LobbyCharacterCast->SetPlayerState(PlayerStateReference);
					if (PlayerMaterials.Num() > (int32)PlayerStateReference->Character * 3 + (int32)PlayerStateReference->CharacterColour)
					{
						LobbyCharacterCast->SetPlayerMat(PlayerMaterials[(int32)PlayerStateReference->Character * 3 + (int32)PlayerStateReference->CharacterColour]);
					}
					
					LobbyCharacterCast->SetPlayerStateRef(PlayerStateReference);
					
					_NewPlayer->Possess(LobbyCharacterCast);
					LobbyCharacterCast->SetOwner(_NewPlayer);
					GameStateReference->SetMaxPlayersOnServer(GetGameInstance<UPrototypeGameInstance>()->MaxPlayersOnServer);
					
					switch(PlayerStateReference->Player_ID)
					{
					case 0:
						{
							if (GameStateReference->GetMaxPlayersOnServer() == 4)
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[0]});
							}
							else if (GameStateReference->GetMaxPlayersOnServer() == 3)
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[1]});
							}
							else
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[2]});
							}
							break;
						}
					case 1:
						{
							if (GameStateReference->GetMaxPlayersOnServer() == 4)
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[2]});
							}
							else if (GameStateReference->GetMaxPlayersOnServer() == 3)
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[3]});
							}
							else
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[4]});
							}
							break;
						}
					case 2:
						{
							if (GameStateReference->GetMaxPlayersOnServer() == 4)
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[4]});
							}
							else
							{
								LobbyCharacterCast->SetActorLocation({PlayerPositions[5]});
							}
							break;
						}
					case 3:
						{
							LobbyCharacterCast->SetActorLocation({PlayerPositions[6]});
							break;
						}
					default:
						
						break;
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
	auto GamestateReference = GetGameState<ALobbyGamestate>();
	if (!GamestateReference)
	{
		return;
	}

	for(auto i = 0; i < GamestateReference->Server_Players.Num(); i++)
	{
		auto PlayerStateReference = GamestateReference->Server_Players[i];
		if (!PlayerStateReference)
		{
			continue;
		}
		
		auto ControllerReference = PlayerStateReference->GetPlayerController();
		if (!ControllerReference)
		{
			continue;
		}

		auto CharacterReference = Cast<ALobbyCharacter>(ControllerReference->GetCharacter());
		if (!CharacterReference)
		{
			continue;
		}
		
		if (PlayerMaterials.Num() <= (int32)PlayerStateReference->CharacterColour)
		{
			continue;
		}
		
		if (PlayerMaterials.Num() > (int32)PlayerStateReference->Character * 4 + (int32)PlayerStateReference->CharacterColour)
			CharacterReference->SetPlayerMat(PlayerMaterials[(int32)PlayerStateReference->Character * 4 + (int32)PlayerStateReference->CharacterColour]);
		else
			CharacterReference->SetPlayerMat(PlayerMaterials[(int32)PlayerStateReference->CharacterColour]);				
	}
	
}


