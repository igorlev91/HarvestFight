#include "RadialPlot.h"
#include "GrowSpot.h"
#include "PlotSign.h"
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

	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void ARadialPlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARadialPlot, PlotSign);
}

void ARadialPlot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDoOnce)
	{
		if (PlotSign)
		{
			bDoOnce = false;
			PlotSign->SetReplicatingMovement(true);
			PlotSign->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PlotSign->SetActorRelativeLocation({});
			PlotSign->SetActorRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
	}
}

void ARadialPlot::Multi_SetPlotMaterial_Implementation(APrototype2PlayerState* _Id)
{
	if (_Id && PlotSignMaterial && !PlotSignMaterialDynamic && PlotSign)
	{
		PlotSignMaterialDynamic = UMaterialInstanceDynamic::Create(PlotSignMaterial, this);
		PlotSignMaterialDynamic->SetVectorParameterValue(FName("PaintColour"), _Id->Details.PureToneColour);
		PlotSign->GetMesh()->SetMaterial(0, PlotSignMaterialDynamic);
	}
}

void ARadialPlot::BeginPlay()
{
	Super::BeginPlay();

	SetReplicatingMovement(true);

	if (!HasAuthority())
		return;

	if (!PlotSignPrefab)
		return;
	
	PlotSign = GetWorld()->SpawnActor<APlotSign>(PlotSignPrefab);

}

void ARadialPlot::SetPlayerID(int32 _Id)
{
	Player_ID = _Id;
	
	SpawnGrowSpots(_Id);
	
	for(auto growSpot : GrowSpots)
	{
		growSpot->OwningPlayerID = _Id;
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

	//if (PlotSignMaterialDynamic)
	//{
	//	PlotSignMesh->SetMaterial(0, PlotSignMaterialDynamic);
	//}
}

int32 ARadialPlot::GetPlayerID() const
{
	return Player_ID;
}

void ARadialPlot::SpawnGrowSpots(int32 _Id)
{
	if (HasAuthority())
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
				newPlot->SetReplicates(true);
				newPlot->SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				newPlot->RadialPlot = this;
				newPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				newPlot->SetActorRelativeLocation({(static_cast<float>(i) + 0.8f) * PlotSpread, (static_cast<float>(j) - 1.0f) * PlotSpread, PlotZHeight});
				newPlot->OwningPlayerID = _Id;
				GrowSpots.Add(newPlot);
			}
		}

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
}

void ARadialPlot::UpdateBeehiveFlowers()
{
	if (GrowSpots.IsEmpty())
		return;
	
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
		if (GrowSpots[i]->GrowingItemRef &&
			GrowSpots[i]->GrowingItemRef->PlantData->PickupType == EPickupDataType::FlowerData)
		{
			for (int j = 0; j < BeehiveVector.size(); j++)
			{
				BeehiveVector.at(j)->NumberOfNearbyFlowers += GrowSpots[i]->GrowingItemRef->PlantData->FlowerValue;
			}
		}
	}
}

