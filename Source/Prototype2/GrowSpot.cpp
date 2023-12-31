// Fill out your copyright notice in the Description page of Project Settings.


#include "GrowSpot.h"
#include "Plant.h"
#include "Prototype2Character.h"
#include "Seed.h"
#include "WeaponSeed.h"

// Sets default values
AGrowSpot::AGrowSpot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrowSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (growingPlant)
	{
		growTime -= GetWorld()->GetDeltaSeconds();
		if (growTime <= 0)
		{
			plantGrown = true;
			growingPlant = false;
		}
	}

}

void AGrowSpot::Interact(APrototype2Character* player)
{
	if (plant)
	{
		if (plantGrown)
		{
			player->HeldItem = plant;
			plant = nullptr;
			plantGrown = false;
		}
	}
}

void AGrowSpot::SetPlant(APlant* _plant, float _growTime)
{
	if (!plant)
	{
		plant = _plant;
		growTime = _growTime;
		growingPlant = true;
	}
}

