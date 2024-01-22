=
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldOverrideData.generated.h"

class UWeaponData;
class UPlantData;

UCLASS()
class PROTOTYPE2_API UWorldOverrideData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<UPlantData*> Plants;

	UPROPERTY(EditDefaultsOnly)
	TArray<UWeaponData*> Weapons;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMesh* SignPostMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMesh* SellBinMesh;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMesh* GrowSpotMesh;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APreGameArena> PreGameArena;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEndGamePodium> EndGamePodium;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACrown> KingCrown;
};
