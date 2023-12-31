#include "Prototype2Gamestate.h"

#include "Net/UnrealNetwork.h"

APrototype2Gamestate::APrototype2Gamestate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2Gamestate::BeginPlay()
{
	Super::BeginPlay();

	
}

void APrototype2Gamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		if (PreviousServerTravel != ShouldServerTravel)
		{
			PreviousServerTravel = ShouldServerTravel;

			if (ShouldServerTravel)
			{
				if (MatchLengthSeconds > 0 || MatchLengthMinutes > 0)
				{
					IsCountingDown = true;
				}
				PreviousServerTravel = false;
				ShouldServerTravel = false;
			}
		}

		if (IsCountingDown)
		{
			if (MatchLengthSeconds > 0)
			{
				MatchLengthSeconds -= DeltaSeconds;
			}
			else
			{
				if (MatchLengthMinutes <= 0)
				{
					// End of timer
					GetWorld()->ServerTravel("Level_MainMenu");
				}
				else
				{
					MatchLengthMinutes--;
					MatchLengthSeconds = 60;
				}
			}
		}
	}
}

void APrototype2Gamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, IsCountingDown);
	DOREPLIFETIME(APrototype2Gamestate, PreviousServerTravel);
	
}

