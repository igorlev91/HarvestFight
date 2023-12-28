#include "LobbyGamestate.h"

#include "Net/UnrealNetwork.h"

ALobbyGamestate::ALobbyGamestate()
{
	PrimaryActorTick.bCanEverTick = true;
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
					GetWorld()->ServerTravel("Level_Main");
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

void ALobbyGamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthMinutes);
	DOREPLIFETIME(ALobbyGamestate, LobbyLengthSeconds);
	DOREPLIFETIME(ALobbyGamestate, IsCountingDown);
	DOREPLIFETIME(ALobbyGamestate, PreviousServerTravel);
}

