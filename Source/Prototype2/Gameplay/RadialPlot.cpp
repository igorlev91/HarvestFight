#include "RadialPlot.h"
#include "GrowSpot.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"

ARadialPlot::ARadialPlot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	PlotSignMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plot Sign"));
	PlotSignMesh->SetupAttachment(RootComponent);
	PlotSignMesh->SetRelativeLocation({-400, -100, 0});
	PlotSignMesh->SetRelativeRotation({0, 180, 0});
	PlotSignMesh->SetIsReplicated(true);

	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void ARadialPlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARadialPlot, PlotSignMesh);
}

void ARadialPlot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDoOnce)
	{
		bDoOnce = false;
		SSComponent->SetMeshesToStretch({PlotSignMesh});
		SSComponent->Enable();
	}
}

void ARadialPlot::Multi_SetPlotMaterial_Implementation(APrototype2PlayerState* _Id)
{
	if (_Id && PlotSignMaterial && !PlotSignMaterialDynamic)
	{
		PlotSignMaterialDynamic = UMaterialInstanceDynamic::Create(PlotSignMaterial, nullptr);
		PlotSignMaterialDynamic->SetVectorParameterValue(FName("PaintColour"), _Id->Details.CharacterColour);
		//PlotSignMaterialDynamic->UpdateCachedData();
		PlotSignMesh->SetMaterial(0, PlotSignMaterialDynamic);
	}
}

void ARadialPlot::BeginPlay()
{
	Super::BeginPlay();
	PlotSignMesh->SetIsReplicated(true);
	
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		//Multi_SetPlotMaterial(Player_ID);
		
		if (!GrowSpotPrefab)
		{
			return;
		}
		
		for(auto i = 0; i < 3; i++)
		{
			for(auto j = 0; j < 3; j++)
			{
				auto newPlot = GetWorld()->SpawnActor<AGrowSpot>(GrowSpotPrefab, RootComponent->GetComponentTransform());
				newPlot->SetRadialReferance(this);
				newPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				newPlot->SetActorRelativeLocation({(static_cast<float>(i) + 0.8f) * PlotSpread, (static_cast<float>(j) - 1.0f) * PlotSpread, PlotZHeight});
				newPlot->Player_ID = Player_ID;
				GrowSpots.Add(newPlot);
			}
		}
	}


}

void ARadialPlot::SetPlayerID(int32 _Id)
{
	Player_ID = _Id;
	for(auto growSpot : GrowSpots)
	{
		growSpot->Player_ID = _Id;
	}
	
	if (HasAuthority())
	{
		if (auto Gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			if (Gamestate->Server_Players.Num() > _Id)
			{
				if (auto PlayerOwner = Gamestate->Server_Players[_Id])
				{
					Multi_SetPlotMaterial(PlayerOwner);
				}
			}
		}
	}

	if (PlotSignMaterialDynamic)
	{
		PlotSignMesh->SetMaterial(0, PlotSignMaterialDynamic);
	}
}

int32 ARadialPlot::GetPlayerID() const
{
	return Player_ID;
}

void ARadialPlot::UpdateBeehiveFlowers()
{

	std::vector<ABeehive*> BeehiveVector;
	for (int i = 0; i < GrowSpots.Num(); i++)
	{
		if (GrowSpots[i]->Beehive)
		{
			BeehiveVector.insert(BeehiveVector.begin(), GrowSpots[i]->Beehive);
			GrowSpots[i]->Beehive->NumberOfNearbyFlowers = 0;
		}
	}

	for (int i = 0; i < GrowSpots.Num(); i++)
	{
		if (!GrowSpots[i]->Plant)
		{
			
		}
		else if (GrowSpots[i]->Plant->PlantData->PickupType == EPickupDataType::FlowerData)
		{
			for (int j = 0; j < BeehiveVector.size(); j++)
			{
				BeehiveVector.at(j)->NumberOfNearbyFlowers += GrowSpots[i]->Plant->PlantData->FlowerValue;
			}
		}
	}
}

