#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"


ALobbyGamemode::ALobbyGamemode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALobbyGamemode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PreviousServerTravel != ShouldServerTravel)
	{
		PreviousServerTravel = ShouldServerTravel;

		if (ShouldServerTravel)
			GetWorld()->ServerTravel("Level_Main");
	}
}
