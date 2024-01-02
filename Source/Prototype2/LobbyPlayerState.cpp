#include "LobbyPlayerState.h"

#include "LobbyCharacter.h"
#include "Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"


void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, IsReady);
	DOREPLIFETIME(ALobbyPlayerState, Player_ID);
	DOREPLIFETIME(ALobbyPlayerState, Character);
	DOREPLIFETIME(ALobbyPlayerState, CharacterColour);
}

void ALobbyPlayerState::SetIsReady(bool _isReady)
{
	IsReady = _isReady;
}

void ALobbyPlayerState::UpdateCharacterMaterial(ECharacters _character, ECharacterColours _characterColour)
{
	Character = _character;
	CharacterColour = _characterColour;

	// Print
	//UE_LOG(LogTemp, Warning, TEXT("Player %s Colour Changed: %s"), *FString::FromInt(Player_ID), *FString::FromInt((int)CharacterColour));
}
