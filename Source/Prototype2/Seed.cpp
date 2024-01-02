// Fill out your copyright notice in the Description page of Project Settings.


#include "Seed.h"

#include "Prototype2Character.h"

ASeed::ASeed()
{
	bReplicates = true;
}

void ASeed::Interact(APrototype2Character* player)
{
	ItemComponent->Interact(player, this);
}

void ASeed::Grow()
{
	
}
