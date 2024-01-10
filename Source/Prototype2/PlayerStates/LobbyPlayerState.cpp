#include "LobbyPlayerState.h"

#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"

ALobbyPlayerState::ALobbyPlayerState()
{
	//CharacterColour = (ECharacterColours)((rand() % 3) + 1);
}

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		FString Name{};
		auto OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem)
		{
			if (auto Interface = OnlineSubsystem->GetIdentityInterface())
			{
				if (auto uniqueID = GetUniqueId().GetUniqueNetId())
				{
					Name = Interface->GetPlayerNickname(*uniqueID);
				}
			}
		}
		GameInstance->FinalPlayerDetails.FindOrAdd(Name, FCharacterDetails{});
	}
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, IsReady);
	DOREPLIFETIME(ALobbyPlayerState, Player_ID);
	DOREPLIFETIME(ALobbyPlayerState, Details);
	DOREPLIFETIME(ALobbyPlayerState, PlayerName);
}

void ALobbyPlayerState::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);

	if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		FString Name{};
		auto OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem)
		{
			if (auto Interface = OnlineSubsystem->GetIdentityInterface())
			{
				if (auto uniqueID = GetUniqueId().GetUniqueNetId())
				{
					Name = Interface->GetPlayerNickname(*uniqueID);
				}
			}
		}
		GameInstance->UniqueNetIDName = Name;
	}
}

void ALobbyPlayerState::SetIsReady(bool _bIsReady)
{
	IsReady = _bIsReady;
}

void ALobbyPlayerState::UpdateCharacterMaterial(FCharacterDetails _Details)
{
	Details = _Details;

	// Print
	//UE_LOG(LogTemp, Warning, TEXT("Player %s Colour Changed: %s"), *FString::FromInt(Player_ID), *FString::FromInt((int)CharacterColour));
}
