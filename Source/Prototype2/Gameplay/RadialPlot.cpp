#include "RadialPlot.h"
#include "GrowSpot.h"
#include "Net/UnrealNetwork.h"

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

void ARadialPlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARadialPlot, PlotSignMesh);
}

void ARadialPlot::Multi_SetPlotMaterial_Implementation(int32 _Id)
{
	if (PlotSignMaterials.Num() > _Id)
		PlotSignMesh->SetMaterial(0, PlotSignMaterials[_Id]);
}

void ARadialPlot::BeginPlay()
{
	Super::BeginPlay();
	PlotSignMesh->SetIsReplicated(true);
	
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (!GrowSpotPrefab)
		{
			return;
		}
		
		for(auto i = 0; i < 3; i++)
		{
			for(auto j = 0; j < 3; j++)
			{
				auto newPlot = GetWorld()->SpawnActor<AGrowSpot>(GrowSpotPrefab, RootComponent->GetComponentTransform());
				newPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				newPlot->SetActorRelativeLocation({(static_cast<float>(i) + 0.8f) * PlotSpread, (static_cast<float>(j) - 1.0f) * PlotSpread, PlotZHeight});
				newPlot->Player_ID = Player_ID;
				growSpots.Add(newPlot);
			}
		}
	}
}

void ARadialPlot::SetPlayerID(int32 _Id)
{
	Player_ID = _Id;
	for(auto growSpot : growSpots)
	{
		growSpot->Player_ID = _Id; 
	}

	if (PlotSignMaterials.Num() > _Id)
		PlotSignMesh->SetMaterial(0, PlotSignMaterials[_Id]);
	
	if (HasAuthority())
	{
		Multi_SetPlotMaterial(_Id);
	}
}

int32 ARadialPlot::GetPlayerID() const
{
	return Player_ID;
}

