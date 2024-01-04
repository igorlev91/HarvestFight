#include "SeedSpawner.h"

#include "NavigationSystem.h"
#include "PlantSeed.h"
#include "Seed.h"
#include "WeaponSeed.h"
#include "AI/NavigationSystemBase.h"
#include "Gamestates/Prototype2Gamestate.h"
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

	if (auto gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		MatchLengthSeconds = (gameState->MatchLengthMinutes * 60) + gameState->MatchLengthSeconds;
		
	}
}

void ASeedSpawner::SpawnSeedsOnTick(float DeltaTime)
{
	float ratioFromCurve{};
	float maxSeedsToSpawn = (float)MaxSeedPackets;
	
	if (auto gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (FloatCurve)
		{
			if (gameState->MatchLengthSeconds > 0)
			{
				int currentMatchLengthSeconds = (gameState->MatchLengthMinutes * 60) + gameState->MatchLengthSeconds;
				ratioFromCurve =  FloatCurve->GetFloatValue(FMath::Lerp(1, 0, (float)currentMatchLengthSeconds / (float)MatchLengthSeconds));
				maxSeedsToSpawn *= ratioFromCurve;
			}
		}
		if (gameState->GameHasStarted && !gameState->HasGameFinished)
		{
			TArray<AActor*> SpawnedSeeds;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlantSeed::StaticClass(), SpawnedSeeds);
			if (SeedPrefabs.Num() > 0 && SpawnedSeeds.Num() < FMath::RoundToInt(maxSeedsToSpawn))
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
						finalLocation.Z = SpawnHeight; // finalLocation.Z = 800.0f; for bens platform map
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
						finalLocation.Z = 100; // finalLocation.Z = 800.0f; for bens platform map
						GetWorld()->SpawnActor<ASeed>(WeaponSeeds[rand() % WeaponSeeds.Num()], finalLocation, {});
						WeaponSpawnTimer = WeaponAverageSpawnTime + rand() % 4;
					}
				}
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

