// Fill out your copyright notice in the Description page of Project Settings.


#include "PlantSeed.h"
#include "Plant.h"
#include "Prototype2Character.h"

void APlantSeed::Interact(APrototype2Character* player)
{
	Super::Interact(player);
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

APlantSeed::APlantSeed()
{
	AttachMesh();
}
