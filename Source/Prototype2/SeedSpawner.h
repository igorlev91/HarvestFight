#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SeedSpawner.generated.h"

class ASeed;
UCLASS()
class PROTOTYPE2_API ASeedSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ASeedSpawner();

protected:
	virtual void BeginPlay() override;

	void SpawnSeedsOnTick(float DeltaTime);

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TArray<TSubclassOf<ASeed>> SeedPrefabs;

	UPROPERTY(EditAnywhere, Category = WeaponSeeds, meta = (AllowPrivateAccess))
	TArray<TSubclassOf<ASeed>> WeaponSeeds;
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
