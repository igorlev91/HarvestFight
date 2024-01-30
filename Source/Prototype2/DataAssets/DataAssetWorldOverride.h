
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAssetWorldOverride.generated.h"


UCLASS()
class PROTOTYPE2_API ADataAssetWorldOverride : public AActor
{
	GENERATED_BODY()
	
	ADataAssetWorldOverride();

public:
	UPROPERTY(EditAnywhere)
	class UWorldOverrideData* WorldOverrideData;

	UPROPERTY(VisibleAnywhere)
	TArray<class UStaticMeshComponent*> PreGameArenaSpawnLocations;
};
