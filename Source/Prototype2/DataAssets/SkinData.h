
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkinData.generated.h"

UCLASS()
class PROTOTYPE2_API USkinData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<class UAnimationData*> Models{};
};
