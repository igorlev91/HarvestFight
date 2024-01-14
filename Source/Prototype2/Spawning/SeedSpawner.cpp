

#include "SeedSpawner.h"

#include "NavigationSystem.h"
#include "Prototype2/Pickups/PlantSeed.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/Pickups/WeaponSeed.h"
#include "AI/NavigationSystemBase.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
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
	for (int i = 0; i < Size; i++)
	{
		CurrentSpawnPos.emplace(CurrentSpawnPos.end(), FVector(0, 0, 0));
		Distances.emplace(Distances.end(), 0);
	}

	if (auto GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		MatchLengthSeconds = (GameState->GetMatchLengthMinutes() * 60) + GameState->GetMatchLengthSeconds();
		
	}
}

void ASeedSpawner::SpawnSeedsOnTick(float _DeltaTime)
{
	float RatioFromCurve{};
	float MaxSeedsToSpawn = static_cast<float>(MaxSeedPackets);
	
	if (auto GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (FloatCurve)
		{
			if (GameState->GetMatchLengthMinutes() > 0)
			{
				int _CurrentMatchLengthSeconds = (GameState->GetMatchLengthMinutes() * 60) + GameState->GetMatchLengthSeconds();
				RatioFromCurve =  FloatCurve->GetFloatValue(FMath::Lerp(1, 0, (float)_CurrentMatchLengthSeconds / (float)MatchLengthSeconds));
				MaxSeedsToSpawn *= RatioFromCurve;
			}
		}
		if (GameState->HasGameStarted() && !GameState->HasGameFinished())
		{
			TArray<AActor*> SpawnedSeeds;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlantSeed::StaticClass(), SpawnedSeeds);
			SpawnPlantSeeds(SpawnedSeeds, _DeltaTime, MaxSeedPackets);
		
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponSeed::StaticClass(), SpawnedSeeds);
			SpawnWeaponSeeds(SpawnedSeeds, _DeltaTime, WeaponMaxSeedPackets);
		}
	}
}

void ASeedSpawner::SpawnPlantSeeds(TArray<AActor*> _SpawnedSeeds, float _DeltaTime, float _MaxSeeds)
{
	float MaxSeedsToSpawn = _MaxSeeds;
	if (!PlantSeedPrefab || _SpawnedSeeds.Num() >= FMath::RoundToInt(MaxSeedsToSpawn))
	{
		return;
	}
	
	SpawnTimer -= _DeltaTime;
	
	if (SpawnTimer > 0)
		return;

	// Spawn Seed
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		for (int i = 0; i < Size; i++)
		{
			FNavLocation Result;
			NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), MaxSpawnRadius, Result);
			CurrentSpawnPos.at(i) = Result.Location;
		}
		FVector FinalLocation = FVector(0, 0, 0);
		int Pos = 0;
		if (PreviousSpawnPos == FVector(0, 0, 0))
		{
			FinalLocation = CurrentSpawnPos.at(0);
		}
		else
		{
			int furthestDistance = 0;
			for (int i = 0; i < Size; i++)
			{
				Distances.at(i) = FVector::Dist(PreviousSpawnPos, CurrentSpawnPos.at(i));
				if (i == 0)
				{
					furthestDistance = i;
				}
				else if (Distances.at(i) > Distances.at(furthestDistance))
				{
					furthestDistance = i;
				}
			}
			FinalLocation = CurrentSpawnPos.at(furthestDistance);
			Pos = furthestDistance;
		}
		FinalLocation += (CurrentSpawnPos.at(Pos) - GetActorLocation()).GetSafeNormal() * MinSpawnRadius;
		FinalLocation.Z = SpawnHeight; // finalLocation.Z = 800.0f; for bens platform map
		//GetWorld()->SpawnActor<ASeed>(SeedPrefabs[rand() % SeedPrefabs.Num()], FinalLocation, {});

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FinalLocation);

		// Spawn plant seed from data asset
		if (PlantDataArray.Num() <= 0)
		{
			return;
		}

		int32 RandomIndex = rand() % PlantDataArray.Num();
		if (PlantDataArray.Num() <= RandomIndex)
		{
			return;
		}

		if (PlantDataArray[RandomIndex] == nullptr)
		{
			return;
		}

		EPickupDataType PlantDataType = PlantDataArray[RandomIndex]->PickupType;

		if (PlantDataType == EPickupDataType::PlantData && PlantSeedPrefab || PlantDataType == EPickupDataType::FlowerData && PlantSeedPrefab)
		{
			if (auto NewSeed = GetWorld()->SpawnActor<ASeed>(PlantSeedPrefab, FinalLocation, {}))
			{
				NewSeed->SetPlantSeedData(PlantDataArray[RandomIndex]);
				NewSeed->SetParachuteMesh(ParachuteMesh);
			}
		}
		else if (PlantDataType == EPickupDataType::BeehiveData && BeehiveBoxPrefab)
		{
			if (auto NewSeed = GetWorld()->SpawnActor<ASeed>(BeehiveBoxPrefab, FinalLocation, {}))
			{
				NewSeed->SetPlantSeedData(PlantDataArray[RandomIndex]);
				NewSeed->SetParachuteMesh(ParachuteMesh);
			}
		}
		
		SpawnTimer = AverageSpawnTime + rand() % 4;
	}
}

void ASeedSpawner::SpawnWeaponSeeds(TArray<AActor*> _SpawnedSeeds, float _DeltaTime, float _MaxSeeds)
{
	float MaxSeedsToSpawn = _MaxSeeds;
	if (!WeaponSeedPrefab || _SpawnedSeeds.Num() >= FMath::RoundToInt(MaxSeedsToSpawn))
	{
		return;
	}
	
	SpawnTimer -= _DeltaTime;
	
	if (SpawnTimer > 0)
		return;

	// Spawn Seed
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		for (int i = 0; i < Size; i++)
		{
			FNavLocation Result;
			NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), WeaponMaxSpawnRadius, Result);
			CurrentSpawnPos.at(i) = Result.Location;
		}
		FVector FinalLocation = FVector(0, 0, 0);
		int Pos = 0;
		if (PreviousSpawnPos == FVector(0, 0, 0))
		{
			FinalLocation = CurrentSpawnPos.at(0);
		}
		else
		{
			int FurthestDistance = 0;
			for (int i = 0; i < Size; i++)
			{
				Distances.at(i) = FVector::Dist(PreviousSpawnPos, CurrentSpawnPos.at(i));
				if (i == 0)
				{
					FurthestDistance = i;
				}
				else if (Distances.at(i) > Distances.at(FurthestDistance))
				{
					FurthestDistance = i;
				}
			}
			FinalLocation = CurrentSpawnPos.at(FurthestDistance);
			Pos = FurthestDistance;
		}
		FinalLocation += (CurrentSpawnPos.at(Pos) - GetActorLocation()).GetSafeNormal() * WeaponMinSpawnRadius;
		FinalLocation.Z = SpawnHeight;

		// Old way of spawning
		//GetWorld()->SpawnActor<AWeaponSeed>(WeaponSeeds[rand() % WeaponSeeds.Num()], FinalLocation, {});
		// Spawn plant seed from data asset
		if (PlantSeedPrefab)
		{
			if (auto NewSeed = GetWorld()->SpawnActor<ASeed>(WeaponSeedPrefab, FinalLocation, {}))
			{
				if (WeaponDataArray.Num() > 0)
				{
					int32 RandomIndex = rand() % WeaponDataArray.Num();
					if (WeaponDataArray.Num() > RandomIndex && WeaponDataArray.Num() > 0)
					{
						if (WeaponDataArray[RandomIndex] != nullptr)
						{
							NewSeed->SetWeaponSeedData(WeaponDataArray[RandomIndex]);
							NewSeed->SetParachuteMesh(ParachuteMesh);
						}
					}
				}
			}
		}
		WeaponSpawnTimer = AverageSpawnTime + rand() % 4;
	}
}

void ASeedSpawner::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	if (HasAuthority())
	{
		SpawnSeedsOnTick(_DeltaTime);
	}
}

