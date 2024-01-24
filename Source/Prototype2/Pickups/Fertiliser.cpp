


#include "Fertiliser.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

AFertiliser::AFertiliser()
{
	bReplicates = true;
}

void AFertiliser::BeginPlay()
{
	Super::BeginPlay();
	PickupActor = EPickupActor::FertilizerActor;
	SetReplicatingMovement(true);
}

void AFertiliser::Interact(APrototype2Character* _Player)
{
	ItemComponent->Interact(_Player, this);

	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);
}

void AFertiliser::HoldInteract(APrototype2Character* _Player)
{
}

void AFertiliser::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);
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
