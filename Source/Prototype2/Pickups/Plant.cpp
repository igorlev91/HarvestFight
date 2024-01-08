


#include "Plant.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/VFX/SquashAndStretch.h"

APlant::APlant()
{
	bReplicates = true;

	InterfaceType = EInterfaceType::Default;
	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
	SSComponent->Disable();
}

void APlant::BeginPlay()
{
	Super::BeginPlay();

	SetReplicatingMovement(true);
}


void APlant::Interact(APrototype2Character* _Player)
{
	if (!bGrown)
	{
		return;
	}
	
	ItemComponent->Interact(_Player, this);

	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	SSComponent->Enable();
}

void APlant::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);

	if (bGrown)
	{
		_Player->UpdateDecalDirection(true, true);
	}
}

void APlant::OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID)
{
	if (!_Owner->HeldItem || _Owner->HeldItem != this)
	{
		_InvokingWidget->SetHUDInteractText("Pick Up");

		_Owner->EnableStencil(true);
	}
}

bool APlant::IsInteractable(APrototype2PlayerState* _Player)
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

void APlant::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}