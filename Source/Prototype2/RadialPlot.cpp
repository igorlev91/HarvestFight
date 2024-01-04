// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialPlot.h"
#include "GrowSpot.h"

// Sets default values
ARadialPlot::ARadialPlot()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARadialPlot::BeginPlay()
{
	Super::BeginPlay();

	if (GrowSpotPrefab)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				if (auto newPlot = GetWorld()->SpawnActor<AGrowSpot>(GrowSpotPrefab, {((float)i - 1.5f) * PlotSpread, ((float)j - 1.5f) * PlotSpread, PlotZHeight}, FRotator::ZeroRotator))
				{
					newPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
					newPlot->Player_ID = Player_ID;
					growSpots.Add(newPlot);
				}
			}
		}
	}
}

void ARadialPlot::SetPlayerID(int _id)
{
	Player_ID = _id;
	for(auto growSpot : growSpots)
	{
		growSpot->Player_ID = _id;
	}
}

