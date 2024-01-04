#include "SeedSpawner.h"

#include "NavigationSystem.h"
#include "PlantSeed.h"
#include "Seed.h"
#include "WeaponSeed.h"
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
	for (int i = 0; i < size; i++)
	{
		currentSpawnPos.emplace(currentSpawnPos.end(), FVector(0, 0, 0));
		distances.emplace(distances.end(), 0);
	}
}

void ASeedSpawner::SpawnSeedsOnTick(float DeltaTime)
{
	TArray<AActor*> SpawnedSeeds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlantSeed::StaticClass(), SpawnedSeeds);
	if (SeedPrefabs.Num() > 0 && SpawnedSeeds.Num() < MaxSeedPackets)
	{
		if (SpawnTimer > 0)
			SpawnTimer -= DeltaTime;
		else 
		{
			// Spawn Seed
			if (UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				for (int i = 0; i < size; i++)
				{
					FNavLocation Result;
					navSys->GetRandomPointInNavigableRadius(GetActorLocation(), MaxSpawnRadius, Result);
					currentSpawnPos.at(i) = Result.Location;
				}
				FVector finalLocation = FVector(0, 0, 0);
				int pos = 0;
				if (previousSpawnPos == FVector(0, 0, 0))
				{
					finalLocation = currentSpawnPos.at(0);
				}
				else
				{
					int furthestDistance = 0;
					for (int i = 0; i < size; i++)
					{
						distances.at(i) = FVector::Dist(previousSpawnPos, currentSpawnPos.at(i));
						if (i == 0)
						{
							furthestDistance = i;
						}
						else if (distances.at(i) > distances.at(furthestDistance))
						{
							furthestDistance = i;
						}
					}
					finalLocation = currentSpawnPos.at(furthestDistance);
					pos = furthestDistance;
				}
				finalLocation += (currentSpawnPos.at(pos) - GetActorLocation()).GetSafeNormal() * MinSpawnRadius;
				finalLocation.Z = 100.0f; // finalLocation.Z = 800.0f; for bens platform map
				GetWorld()->SpawnActor<ASeed>(SeedPrefabs[rand() % SeedPrefabs.Num()], finalLocation, {});
				SpawnTimer = AverageSpawnTime + rand() % 4;
			}
		}
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponSeed::StaticClass(), SpawnedSeeds);
	if (WeaponSeeds.Num() > 0 && SpawnedSeeds.Num() < WeaponMaxSeedPackets)
	{
		if (WeaponSpawnTimer > 0)
			WeaponSpawnTimer -= DeltaTime;
		else 
		{
			// Spawn Seed
			if (UNavigationSystemV1* navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				FNavLocation Result;
				navSys->GetRandomPointInNavigableRadius(GetActorLocation(), WeaponMaxSpawnRadius, Result);
				FVector finalLocation = Result.Location;
				finalLocation += (Result.Location - GetActorLocation()).GetSafeNormal() * WeaponMinSpawnRadius;
				finalLocation.Z = 100.0f; // finalLocation.Z = 800.0f; for bens platform map
				GetWorld()->SpawnActor<ASeed>(WeaponSeeds[rand() % WeaponSeeds.Num()], finalLocation, {});
				WeaponSpawnTimer = WeaponAverageSpawnTime + rand() % 4;
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

