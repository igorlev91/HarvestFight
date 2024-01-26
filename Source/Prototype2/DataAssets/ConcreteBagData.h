

#pragma once

#include "CoreMinimal.h"
#include "FertiliserData.h"
#include "ConcreteBagData.generated.h"

UCLASS()
class PROTOTYPE2_API UConcreteBagData : public UFertiliserData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Properties")
	int32 MaxStrength;
};
