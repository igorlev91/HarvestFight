
#include "Prototype2/Pickups/Beehive.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/BeehiveData.h"

void ABeehive::BeginPlay()
{
	Super::BeginPlay();
}

void ABeehive::Interact(APrototype2Character* _Player)
{
	if (!bIsReadyToCollect)
	{
		return;
	}

	if (!BeehiveData->Honey)
	{
		return;
	}
	APlant* Honey = GetWorld()->SpawnActor<APlant>(Plant, FVector(0, 0, 0), {});
	Honey->PlantData = BeehiveData->Honey;
	Honey->SetPlantData(Honey->PlantData);
	Honey->bGrown = true;
	Honey->NumberOfNearbyFlowers = NumberOfNearbyFlowers;
	
	Honey->Interact(_Player);

	bIsReadyToCollect = false;
	TrackerTimeTillCollect = 0;
}

void ABeehive::ClientInteract(APrototype2Character* _Player)
{
	Super::ClientInteract(_Player);
}

void ABeehive::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	Super::OnDisplayInteractText(_InvokingWidget, _Owner, _PlayerID);
}

bool ABeehive::IsInteractable(APrototype2PlayerState* _Player)
{
	return Super::IsInteractable(_Player);
}

void ABeehive::SetBeehiveData(UBeehiveData* _Data)
{
	BeehiveData = _Data;
	ItemComponent->InitializePlant(_Data->PlantMesh);
	PickupActor = EPickupActor::BeehiveActor;
	DataAssetPickupType = EPickupDataType::BeehiveData;
}

void ABeehive::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsReadyToCollect)
		return;
	
	TrackerTimeTillCollect += DeltaSeconds;

	if (TrackerTimeTillCollect > TimeTillCollect)
	{
		bIsReadyToCollect = true;
	}
}
