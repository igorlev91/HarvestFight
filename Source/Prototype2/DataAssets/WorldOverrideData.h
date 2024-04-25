
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "WorldOverrideData.generated.h"

class UWeaponData;
class UPlantData;

UCLASS()
class PROTOTYPE2_API UWorldOverrideData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<class USeedData*> PlantSeeds;

	UPROPERTY(EditDefaultsOnly)
	TArray<class USeedData*> WeaponSeeds;

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
	TSubclassOf<class AEndGamePodium> EndGamePodium_Teams;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACrown> KingCrown;

	/* Game modifier values */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GameTimeShort;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GameTimeMedium;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GameTimeLong;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="SFX")
	USoundCue* Ambience;
};
