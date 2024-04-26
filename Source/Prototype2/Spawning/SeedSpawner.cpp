

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
#include "Kismet/KismetArrayLibrary.h"

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

	SetUpSeedBag();
}

void ASeedSpawner::SetUpSeedBag()
{
	for (int i = 0; i < NumberOfSeedsPerStarValue.Num(); i++)
	{
		for (int k = 0; k < NumberOfSeedsPerStarValue[i]; k++)
		{
			SeedBag.Add(i);
		}
	}

	ShuffleSeedBag();
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
	SpawnSeeds_RAW();
}

void ASeedSpawner::SpawnSeeds_RAW()
{
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

			int32 RandomIndex;
			if (!bUseBagSystem)
			{
				RandomIndex = rand() % PlantDataArray.Num();
				if (PlantDataArray.Num() <= RandomIndex)
				{
					return;
				}
			}
			else
			{
				if (BagPositionTracker >= SeedBag.Num())
				{
					ShuffleSeedBag();
				}
				RandomIndex = SeedBag[BagPositionTracker];
				BagPositionTracker++;
			}

			if (PlantDataArray[RandomIndex] == nullptr)
			{
				return;
			}

			EPickupDataType PlantDataType = PlantDataArray[RandomIndex]->BabyType;

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

void ASeedSpawner::ShuffleSeedBag()
{
	BagPositionTracker = 0;
	if (SeedBag.Num() <= 0)
		return;

	int32 LastIndex = SeedBag.Num() - 1;
	for (int32 i = 0; i <= LastIndex; i++)
	{
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			SeedBag.Swap(i, Index);
		}
	}
}

void ASeedSpawner::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!HasAuthority())
		return;

	TArray<int32> DeadSeeds{};
	
	SpawnSeedsOnTick(_DeltaTime);

	for(int i = 0; i < SpawnedSeeds.Num(); i++)
	{
		if (!IsValid(SpawnedSeeds[i]))
		{
			DeadSeeds.Add(i);
		}
	}

	for (int32 index : DeadSeeds)
	{
		SpawnedSeeds.RemoveAt(index);
	}
}

