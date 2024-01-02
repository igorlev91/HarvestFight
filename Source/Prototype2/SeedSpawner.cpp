#include "SeedSpawner.h"

#include "NavigationSystem.h"
#include "Seed.h"
#include "AI/NavigationSystemBase.h"
#include "Kismet/GameplayStatics.h"

class UNavigationSystemV1;

ASeedSpawner::ASeedSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ASeedSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASeedSpawner::SpawnSeedsOnTick(float DeltaTime)
{
	TArray<AActor*> SpawnedSeeds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASeed::StaticClass(), SpawnedSeeds);
	if (SeedPrefabs.Num() > 0 && SpawnedSeeds.Num() < MaxSeedPackets)
	{
		if (SpawnTimer > 0)
			SpawnTimer -= DeltaTime;
		else 
		{
			// Spawn Seed
			if (UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				FNavLocation Result;
				navSys->GetRandomPointInNavigableRadius(GetActorLocation(), MaxSpawnRadius, Result);
				FVector finalLocation = Result.Location;
				finalLocation += (Result.Location - GetActorLocation()).GetSafeNormal() * MinSpawnRadius;
				finalLocation.Z = 100.0f;
				GetWorld()->SpawnActor<ASeed>(SeedPrefabs[rand() % SeedPrefabs.Num()], finalLocation, {});
				SpawnTimer = AverageSpawnTime + rand() % 4;
			}
		}
	}
	
}

void ASeedSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		SpawnSeedsOnTick(DeltaTime);
	}
}

