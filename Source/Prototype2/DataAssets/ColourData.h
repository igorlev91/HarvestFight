
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ColourData.generated.h"

UCLASS()
class PROTOTYPE2_API UColourData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> PureColours
	{
		{199.0f/ 255.0f, 50.0f/ 255.0f, 35.0f/ 255.0f, 1.0f},
		{14.0f/ 255.0f, 90.0f/ 255.0f, 209.0f/ 255.0f, 1.0f},
		{55.0f/ 255.0f, 181.0f/ 255.0f, 21.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 193.0f/ 255.0f, 9.0f/ 255.0f, 1.0f},
		{110.0f/ 255.0f, 35.0f/ 255.0f, 199.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 120.0f/ 255.0f, 24.0f/ 255.0f, 1.0f},
		{0.0f/ 255.0f, 0.0f/ 255.0f, 0.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 255.0f/ 255.0f, 255.0f/ 255.0f, 1.0f}
	};
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> Reds
	{
		{228.0f/255.0f, 154.0f/ 255.0f,154.0f / 255.0f,1},
		{239.0f/255.0f, 193.0f/ 255.0f,195.0f / 255.0f,1},
		{228.0f/255.0f, 154.0f/ 255.0f,154.0f / 255.0f,1},
		{228.0f/255.0f, 154.0f/ 255.0f,154.0f / 255.0f,1},
		{228.0f/255.0f, 154.0f/ 255.0f,154.0f / 255.0f,1}
	};
	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> SubReds
	{
		{175.0f/255.0f,90.0f/ 255.0f,90.0f / 255.0f,1},
		{255.0f/255.0f,176.0f/ 255.0f,191.0f / 255.0f,1},
		{175.0f/255.0f,90.0f/ 255.0f,90.0f / 255.0f,1},
		{175.0f/255.0f,90.0f/ 255.0f,90.0f / 255.0f,1},
		{175.0f/255.0f,90.0f/ 255.0f,90.0f / 255.0f,1}
	};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Blue{180.0f/ 255.0f,211.0f/255.0f,222.0f/255.0f,1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor SubBlue{53.0f/ 255.0f,96.0f/255.0f,121.0f/255.0f,1};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Green{211.0f/255.0f,232.0f/255.0f,167.0f/255.0f,1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor SubGreen{79.0f/255.0f,121.0f/255.0f,66.0f/255.0f,1};

	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> Yellows
	{
			{0.828125,0.682099,0.176839,1.000000},
			{0.828125,0.682099,0.176839,1.000000},
			{0.828125,0.682099,0.176839,1.000000},
			{0.828125,0.682099,0.176839,1.000000},
			{0.828125,0.682099,0.176839,1.000000}
	};
	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> SubYellows
	{
			{0.318547,0.181164,0.031896,1.000000},
			{0.318547,0.181164,0.031896,1.000000},
			{0.318547,0.181164,0.031896,1.000000},
			{0.318547,0.181164,0.031896,1.000000},
			{0.318547,0.181164,0.031896,1.000000}
	};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Purple{201.0f/255.0f,184.0f/255.0f,233.0f/255.00f,1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor SubPurple{117.0f/255.0f,101.0f/255.0f,138.0f/255.00f,1};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Orange{255.0f/255.0f,169.0f/255.0f,91.0f/255.0f,1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor SubOrange{185.0f/255.0f,114.0f/255.0f,0.0f/255.0f,1};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Black{97.0f/255.0f,97.0f/255.0f,97.0f/255.0f,1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor SubBlack{153.0f/255.0f,153.0f/255.0f,153.0f/255.0f,1};
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> Whites
	{
		{255.0f/ 255.0f, 255.0f/ 255.0f, 255.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 249.0f/ 255.0f, 251.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 255.0f/ 255.0f, 255.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 244.0f/ 255.0f, 234.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 244.0f/ 255.0f, 234.0f/ 255.0f, 1.0f}
	};
	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> SubWhites
	{
		{0.0f/ 255.0f, 0.0f/ 255.0f, 0.0f/ 255.0f, 1.0f},
		{237.0f/ 255.0f, 200.0f/ 255.0f, 200.0f/ 255.0f, 1.0f},
		{255.0f/ 255.0f, 172.0f/ 255.0f, 60.0f/ 255.0f, 1.0f},
		{228.0f/ 255.0f, 114.0f/ 255.0f, 43.0f/ 255.0f, 1.0f},
		{228.0f/ 255.0f, 114.0f/ 255.0f, 43.0f/ 255.0f, 1.0f}
	};
};
