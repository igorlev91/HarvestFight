#include "LobbyPlayerState.h"

#include "Net/UnrealNetwork.h"


void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, IsReady);
	DOREPLIFETIME(ALobbyPlayerState, Player_ID);
}

void ALobbyPlayerState::SetIsReady(bool _isReady)
{
	IsReady = _isReady;
}