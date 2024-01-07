// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Plant.h"
#include "Prototype2/DataAssets/PlantData.h"
#include "BeehiveData.generated.h"

UCLASS()
class PROTOTYPE2_API UBeehiveData : public UPlantData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category= "Appearance")
	UPlantData* Honey;

};
