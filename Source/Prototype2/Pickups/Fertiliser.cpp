


#include "Fertiliser.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"

AFertiliser::AFertiliser()
{
	bReplicates = true;
}

void AFertiliser::BeginPlay()
{
	Super::BeginPlay();
	PickupActor = EPickupActor::FertilizerActor;
	SetReplicatingMovement(true);

	if (SeedData)
		SetActorScale3D(SeedData->BabyScale);

	ItemComponent->SetStencilEnabled(false);
}

void AFertiliser::Interact(APrototype2Character* _Player)
{
	ItemComponent->Interact(_Player, this);


}

void AFertiliser::HoldInteract(APrototype2Character* _Player)
{
}

void AFertiliser::ClientInteract(APrototype2Character* _Player)
{
	_Player->EnableStencil(false);
	
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	SSComponent->Boing();
}

void AFertiliser::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	if (!_Owner->HeldItem || _Owner->HeldItem != this)
	{
		_InvokingWidget->SetHUDInteractText("Pick Up");

		_Owner->EnableStencil(true);
	}
}

bool AFertiliser::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;

	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Casted = Cast<APrototype2Character>(Character))
			{
				if (!Casted->HeldItem || Casted->HeldItem != this)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}
