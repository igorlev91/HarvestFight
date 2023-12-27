// Fill out your copyright notice in the Description page of Project Settings.


#include "Seed.h"

#include "Prototype2Character.h"

void ASeed::Interact(APrototype2Character* player)
{
	player->HeldItem = this;
}

void ASeed::Grow()
{
	
}
