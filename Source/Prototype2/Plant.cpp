// Fill out your copyright notice in the Description page of Project Settings.


#include "Plant.h"
#include "Prototype2Character.h"

void APlant::Interact(APrototype2Character* player)
{
	player->HeldItem = this;
}

APlant::APlant()
{
	AttachMesh();
}
