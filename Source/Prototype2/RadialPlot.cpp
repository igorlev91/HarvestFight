// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialPlot.h"
#include "GrowSpot.h"

// Sets default values
ARadialPlot::ARadialPlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARadialPlot::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (int i = 0; i < AttachedActors.Num(); i++)
	{
		if (auto* plot = Cast<AGrowSpot>(AttachedActors[i]))
		{
			growSpots[i] = plot;
		}
	}
	
}

// Called every frame
void ARadialPlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARadialPlot::SetPlayerID(int ID)
{
	for (int i = 0; i < growSpots.Num(); i++)
	{
		growSpots[i]->Player_ID = ID;
	}
}

