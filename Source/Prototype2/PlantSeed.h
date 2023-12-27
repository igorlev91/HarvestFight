// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Seed.h"
#include "PlantSeed.generated.h"

/**
 * 
 */
class APlant;
UCLASS()
class PROTOTYPE2_API APlantSeed : public ASeed
{
	GENERATED_BODY()
public:
	virtual void Interact(APrototype2Character* player) override;
	virtual void Grow() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	APlant* plantToGrow = nullptr;
};
