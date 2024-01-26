

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SellBinData.generated.h"

UCLASS()
class PROTOTYPE2_API USellBinData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	UStaticMesh* Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	FVector DesiredScale;
};
