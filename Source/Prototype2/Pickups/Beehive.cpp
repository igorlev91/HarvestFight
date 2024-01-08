

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
	APlant* Honey = GetWorld()->SpawnActor<APlant>(FVector(0, 0, 0), {});
	Honey->PlantData = BeehiveData->Honey;

	Honey->PlantData->SellValue *= NumberOfNearbyFlowers + 1;
	ItemComponent->Interact(_Player, Honey);

	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);
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
