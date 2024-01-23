

#include "SeedSpawner.h"

#include "NavigationSystem.h"
#include "Prototype2/Pickups/PlantSeed.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/Pickups/WeaponSeed.h"
#include "AI/NavigationSystemBase.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/SeedData.h"

class UNavigationSystemV1;

ASeedSpawner::ASeedSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ASeedSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;
	
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
			SpawnSeeds(SpawnedSeeds, _DeltaTime, MaxSeedPackets);
		}
	}
}

void ASeedSpawner::SpawnSeeds(TArray<AActor*> _SpawnedSeeds, float _DeltaTime, float _MaxSeeds)
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
		FNavLocation Result{};
		if (NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), MaxSpawnRadius, Result))
		{
			Result.Location.Z = SpawnHeight;
			
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Result.Location);

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

			EPickupDataType PlantDataType = PlantDataArray[RandomIndex]->Type;

			if (PlantDataType == EPickupDataType::PlantData && PlantSeedPrefab || PlantDataType == EPickupDataType::FlowerData && PlantSeedPrefab)
			{
				if (auto NewSeed = GetWorld()->SpawnActor<ASeed>(PlantSeedPrefab, Result.Location, FRotator::ZeroRotator))
				{
					NewSeed->SetSeedData(PlantDataArray[RandomIndex], EPickupActor::SeedActor);
					NewSeed->SetParachuteMesh(ParachuteMesh);
					NewSeed->SetActorRelativeRotation(FRotator::ZeroRotator);
					SpawnedSeeds.Add(NewSeed);
				}
			}
			else if (PlantDataType == EPickupDataType::BeehiveData && BeehiveBoxPrefab)
			{
				if (auto NewSeed = GetWorld()->SpawnActor<ASeed>(BeehiveBoxPrefab, Result.Location, FRotator::ZeroRotator))
				{
					NewSeed->SetSeedData(PlantDataArray[RandomIndex], EPickupActor::SeedActor);
					NewSeed->SetParachuteMesh(ParachuteMesh);
					NewSeed->SetActorRelativeRotation(FRotator::ZeroRotator);
					SpawnedSeeds.Add(NewSeed);
				}
			}
			else if (PlantDataType == EPickupDataType::WeaponData && WeaponSeedPrefab)
			{
				if (auto NewSeed = GetWorld()->SpawnActor<AWeaponSeed>(WeaponSeedPrefab, Result.Location, FRotator::ZeroRotator))
				{
					NewSeed->SetSeedData(PlantDataArray[RandomIndex], EPickupActor::SeedActor);
					NewSeed->SetParachuteMesh(ParachuteMesh);
					NewSeed->SetActorRelativeRotation(FRotator::ZeroRotator);
					SpawnedSeeds.Add(NewSeed);
				}
			}
		
			SpawnTimer = AverageSpawnTime + rand() % 4;
		}
		
	}
}

void ASeedSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASeedSpawner, SpawnedSeeds);
}

void ASeedSpawner::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!HasAuthority())
		return;
	

	SpawnSeedsOnTick(_DeltaTime);
	for(auto Seed : SpawnedSeeds)
	{
		if (!IsValid(Seed))
		{
			SpawnedSeeds.Remove(Seed);
		}
	}
}

