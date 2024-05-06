

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
	friend class URandomEventManager;
	GENERATED_BODY()
	
public:	
	ASeedSpawner();

protected:
	/* sets up the seed spawner */
	virtual void BeginPlay() override;

	void SetUpSeedBag();

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
	void SpawnSeeds(TArray<AActor*> _SpawnedSeeds, float _DeltaTime, float _MaxSeeds);
	void SpawnSeeds_RAW();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	void ShuffleSeedBag();
public:
	
	virtual void Tick(float _DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Spawn Amount Curve")
	class UCurveFloat* FloatCurve;
private:

	/* Spawning seed from PlantData Data Asset */
	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TArray<USeedData*> PlantDataArray;	
	UPROPERTY(EditDefaultsOnly, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<ASeed> PlantSeedPrefab;
	UPROPERTY(EditDefaultsOnly, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<class AWeaponSeed> WeaponSeedPrefab;
	UPROPERTY(EditDefaultsOnly, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<ASeed> BeehiveBoxPrefab;
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<AActor*> SpawnedSeeds;
	UPROPERTY(EditAnywhere, Category = Parachute, meta = (AllowPrivateAccess))
	UStaticMesh* ParachuteMesh;
	
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

	// bag system
	UPROPERTY(EditAnywhere)
	bool bUseBagSystem = false;
	UPROPERTY(EditAnywhere)
	bool bUseWeightedSystem = false;
	UPROPERTY(EditAnywhere)
	TArray<int32> NumberOfSeedsPerStarValue;
	UPROPERTY(VisibleAnywhere)
	int32 BagPositionTracker;
	UPROPERTY(VisibleAnywhere)
	TArray<int32> SeedBag;
	
};
