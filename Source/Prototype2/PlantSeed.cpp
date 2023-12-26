// Fill out your copyright notice in the Description page of Project Settings.


#include "PlantSeed.h"
#include "Plant.h"

void APlantSeed::Interact()
{
	Super::Interact();
}

void APlantSeed::Grow()
{
	if (growtime <= 0)
	{
		// Spawn Plant
		// Destroy This Object
	}
}

void APlantSeed::Tick(float DeltaSeconds)
{
	if (isPlanted)
	{
		growtime -= GetWorld()->GetDeltaSeconds();
		Grow();
	}
}
