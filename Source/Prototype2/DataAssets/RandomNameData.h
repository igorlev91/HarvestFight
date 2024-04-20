

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RandomNameData.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API URandomNameData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FString> RandomNames
	{
		"FarmLife",
		"AgriStar",
		"SoilMan",
		"CropBoss",
		"BarnKing",
		"SeedGuru",
		"FarmTech",
		"GrowWiz",
		"MilkMaid",
		"FarmHand",
		"HayBales",
		"PlowBoy",
		"RanchGal",
		"AgriChic",
		"EcoFarm",
		"TractorUp",
		"VegPatch",
		"FertMix",
		"HerdCare",
		"MooCrew"
	};
};
