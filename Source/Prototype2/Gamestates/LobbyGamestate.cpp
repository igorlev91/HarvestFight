#include "LobbyGamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/LobbyPlayerState.h"

ALobbyGamestate::ALobbyGamestate()
{
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = true;
}

void ALobbyGamestate::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (HasAuthority())
	{
		
		if (PreviousServerTravel != ShouldServerTravel)
		{
			PreviousServerTravel = ShouldServerTravel;

			if (ShouldServerTravel)
			{
				if (LobbyLengthSeconds > 0 || LobbyLengthMinutes > 0)
				{
					IsCountingDown = true;
				}
				PreviousServerTravel = false;
				ShouldServerTravel = false;
			}
		}

		if (IsCountingDown)
		{
			if (LobbyLengthSeconds > 0)
			{
				LobbyLengthSeconds -= DeltaSeconds;
			}
			else
			{
				if (LobbyLengthMinutes <= 0)
				{
					// End of timer
					GetWorld()->ServerTravel("Level_Main", false, false);
				}
				else
				{
					LobbyLengthMinutes--;
					LobbyLengthSeconds = 60;
				}
			}
		}
	}
}

void ALobbyGamestate::SetIsReady(int _player, bool _isReady)
{
	if (Server_Players.Num() >= _player)
	{
		if (auto playerState = Server_Players[_player])
		{
			playerState->SetIsReady(_isReady);
		}
	}

	bool isEveryoneReady{true};
	for(auto player : Server_Players)
	{
		if (!player->IsReady)
			isEveryoneReady = false;
	}
	
	if (isEveryoneReady && Server_Players.Num() >= 1)
	{
		ShouldServerTravel = true;
		LobbyLengthSeconds = 7.0f;
	}
	else
	{
		ShouldServerTravel = false;
		IsCountingDown = false;
		PreviousServerTravel = false;
		LobbyLengthSeconds = 7.0f;
	}
}

void ALobbyGamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthMinutes);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, IsCountingDown);
	DOREPLIFETIME(ALobbyGamestate, PreviousServerTravel);
	DOREPLIFETIME(ALobbyGamestate, Server_Players);
}

