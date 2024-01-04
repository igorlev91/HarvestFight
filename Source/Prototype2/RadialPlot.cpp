// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialPlot.h"
#include "GrowSpot.h"

// Sets default values
ARadialPlot::ARadialPlot()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	PlotSignMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plot Sign"));
	PlotSignMesh->SetupAttachment(RootComponent);
	PlotSignMesh->SetRelativeLocation({-400, -100, 0});
	PlotSignMesh->SetRelativeRotation({0, 180, 0});
	PlotSignMesh->SetIsReplicated(true);
}

void ARadialPlot::Multi_SetPlotMaterial_Implementation(int _id)
{
	if (PlotSignMaterials.Num() > _id)
		PlotSignMesh->SetMaterial(0, PlotSignMaterials[_id]);
}

// Called when the game starts or when spawned
void ARadialPlot::BeginPlay()
{
	Super::BeginPlay();
	PlotSignMesh->SetIsReplicated(true);
	
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (GrowSpotPrefab)
		{
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					if (auto newPlot = GetWorld()->SpawnActor<AGrowSpot>(GrowSpotPrefab, RootComponent->GetComponentTransform()))
					{
						newPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
						newPlot->SetActorRelativeLocation({((float)i - 1.5f) * PlotSpread, ((float)j - 1.5f) * PlotSpread, PlotZHeight});
						newPlot->Player_ID = Player_ID;
						growSpots.Add(newPlot);
					}
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

	if (PlotSignMaterials.Num() > _id)
		PlotSignMesh->SetMaterial(0, PlotSignMaterials[_id]);
	
	if (HasAuthority())
	{
		Multi_SetPlotMaterial(_id);
	}
}

