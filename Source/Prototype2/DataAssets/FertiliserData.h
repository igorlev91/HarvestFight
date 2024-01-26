
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupTypeEnum.h"

#include "FertiliserData.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UFertiliserData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Fertiliser Type")
	bool bConcrete;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Fertiliser Spawning")
	int32 MaxSpawnedAtAnyGivenTime;
};
