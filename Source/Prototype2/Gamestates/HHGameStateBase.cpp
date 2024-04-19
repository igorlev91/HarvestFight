

#include "HHGameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"

void AHHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHHGameStateBase, GameMode);
}

void AHHGameStateBase::SetGameMode(int32 _Mode)
{
	GameMode = _Mode;
}

int32 AHHGameStateBase::GetGameMode()
{
	return GameMode;
}

void AHHGameStateBase::OnRep_Gamemode()
{
}
