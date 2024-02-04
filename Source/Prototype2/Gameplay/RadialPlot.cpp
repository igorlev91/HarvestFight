#include "RadialPlot.h"
#include "GrowSpot.h"
#include "PlotSign.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/DataAssets/SeedData.h"

ARadialPlot::ARadialPlot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));


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

		SignColour = _Id->Details.Colour;
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

void ARadialPlot::SetPlayerID(EColours _PlayerName)
{
	PlayerColour = _PlayerName;
	
	SpawnGrowSpots(_PlayerName);
	
	for(auto growSpot : GrowSpots)
	{
		growSpot->OwningPlayerColor = _PlayerName;
	}
	
	if (HasAuthority())
	{
		if (auto Gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			for(auto Player : Gamestate->Server_Players)
			{
				if (Player->Details.Colour == _PlayerName)
				{
					Multi_SetPlotMaterial(Player);
				}
			}
		}
	}

	//if (PlotSignMaterialDynamic)
	//{
	//	PlotSignMesh->SetMaterial(0, PlotSignMaterialDynamic);
	//}
}

EColours ARadialPlot::GetPlayerID() const
{
	return PlayerColour;
}

void ARadialPlot::SpawnGrowSpots(EColours _PlayerColour)
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
				newPlot->SetActorRotation(PlotSign->GetActorRotation());
				newPlot->OwningPlayerColor = _PlayerColour;
				GrowSpots.Add(newPlot);
			}
		}

		if (HasAuthority())
		{
			if (auto Gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
			{
				for(auto Player : Gamestate->Server_Players)
				{
					if (Player->Details.Colour == _PlayerColour)
					{
						Multi_SetPlotMaterial(Player);
					}
				}
			}
		}
	}
}

void ARadialPlot::UpdateBeehiveFlowers()
{
	if (GrowSpots.Num() > 0)
		return;
	
	TArray<ABeehive*> BeehiveVector;
	for (int32 i = 0; i < GrowSpots.Num(); i++)
	{
		if (GrowSpots[i]->Beehive)
		{
			BeehiveVector.Add(GrowSpots[i]->Beehive);
			GrowSpots[i]->Beehive->NumberOfNearbyFlowers = 0;
		}
	}

	for (int32 i = 0; i < GrowSpots.Num(); i++)
	{
		if (BeehiveVector.Num() > 0 && GrowSpots[i]->GrowingItemRef &&
			(GrowSpots[i]->GrowingItemRef->SeedData->BabyType == EPickupDataType::FlowerData
			|| GrowSpots[i]->GrowingItemRef->SeedData->BabyType == EPickupDataType::PlantData))
		{
			for (ABeehive* Behive : BeehiveVector)
			{
				Behive->NumberOfNearbyFlowers++;
			}
		}
	}
}

