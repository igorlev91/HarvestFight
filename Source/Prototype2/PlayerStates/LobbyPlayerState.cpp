#include "LobbyPlayerState.h"

#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"

ALobbyPlayerState::ALobbyPlayerState()
{
	CharacterColour = (ECharacterColours)((rand() % 3) + 1);
}

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, IsReady);
	DOREPLIFETIME(ALobbyPlayerState, Player_ID);
	DOREPLIFETIME(ALobbyPlayerState, Character);
	DOREPLIFETIME(ALobbyPlayerState, CharacterColour);
	DOREPLIFETIME(ALobbyPlayerState, PlayerName);
}

void ALobbyPlayerState::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
}

void ALobbyPlayerState::SetIsReady(bool _bIsReady)
{
	IsReady = _bIsReady;
}

void ALobbyPlayerState::UpdateCharacterMaterial(ECharacters _Character, ECharacterColours _CharacterColour)
{
	Character = _Character;
	CharacterColour = _CharacterColour;
	
	// Print
	//UE_LOG(LogTemp, Warning, TEXT("Player %s Colour Changed: %s"), *FString::FromInt(Player_ID), *FString::FromInt((int)CharacterColour));
}
