

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/DataAssets/PlantData.h"
#include "GameFramework/Actor.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "SeedSpawner.generated.h"

class ASeed;
UCLASS()
class PROTOTYPE2_API ASeedSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASeedSpawner();

protected:
	/* sets up the seed spawner */
	virtual void BeginPlay() override;

	/**
	 * @brief calls the spawn seeds functions
	 * @param _DeltaTime 
	 */
	void SpawnSeedsOnTick(float _DeltaTime);

	/**
	 * @brief manages the spawning of the plant seeds
	 * @param _SpawnedSeeds 
	 * @param _DeltaTime 
	 * @param _MaxSeeds 
	 */
	void SpawnPlantSeeds(TArray<AActor*> _SpawnedSeeds, float _DeltaTime, float _MaxSeeds);
	/**
	 * @brief Manages the spawning of the weapon seeds
	 * @param _SpawnedSeeds 
	 * @param _DeltaTime 
	 * @param _MaxSeeds 
	 */
	void SpawnWeaponSeeds(TArray<AActor*> _SpawnedSeeds, float _DeltaTime, float _MaxSeeds);

	void SpawnFertiliser(TArray<AActor*> _SpawnedFertiliser, float _DeltaTime, float _MaxFertiliser);

public:
	
	virtual void Tick(float _DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Spawn Amount Curve")
	class UCurveFloat* FloatCurve;
private:

	/* Spawning seed from PlantData Data Asset */
	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TArray<UPlantData*> PlantDataArray;	
	UPROPERTY(EditDefaultsOnly, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<ASeed> PlantSeedPrefab;
	UPROPERTY(EditDefaultsOnly, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<ASeed> BeehiveBoxPrefab;

	/* Spawning seed from WeaponData Data Asset */
	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TArray<UWeaponData*> WeaponDataArray;	
	UPROPERTY(EditDefaultsOnly, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<ASeed> WeaponSeedPrefab;

	UPROPERTY(EditAnywhere, Category = Parachute, meta = (AllowPrivateAccess))
	UStaticMesh* ParachuteMesh;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AFertiliser> FertiliserAsset;
	
	FVector PreviousSpawnPos = FVector(0, 0, 0);
	std::vector<FVector> CurrentSpawnPos;
	std::vector<float> Distances;
	int Size = 3;

	int MatchLengthSeconds{};
	int CurrentMatchLengthSeconds{};

	UPROPERTY(EditAnywhere)
	int SpawnHeight{1200};

private:
	float SpawnTimer{};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	int32 MaxSeedPackets{5};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float AverageSpawnTime{1.0f};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float MinSpawnRadius{100.0f};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float MaxSpawnRadius{800.0f};

	float FertiliserSpawnTimer{};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	int32 MaxFertilser{5};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float FertiliserAverageSpawnTime{10.0f};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))

	float WeaponSpawnTimer{};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	int32 WeaponMaxSeedPackets{3};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float WeaponAverageSpawnTime{2.0f};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float WeaponMinSpawnRadius{100.0f};
	UPROPERTY(EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess))
	float WeaponMaxSpawnRadius{800.0f};
};
