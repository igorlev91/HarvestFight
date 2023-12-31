// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Prototype2Character.h"

APlant::APlant()
{
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
}


void APlant::Interact(APrototype2Character* player)
{
	ItemComponent->Interact(player, this);
}
