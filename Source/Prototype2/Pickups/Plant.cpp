
#include "Plant.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

APlant::APlant()
{
	bReplicates = true;
	//LeavesMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Leaves Mesh"));
	//LeavesMesh->SetupAttachment(RootComponent);

	InterfaceType = EInterfaceType::Default;
}

void APlant::BeginPlay()
{
	Super::BeginPlay();
	
	Server_ToggleGold();

	SetReplicatingMovement(true);

	/*LeavesMesh->SetupAttachment(RootComponent);
	LeavesMesh->SetCollisionProfileName(FName("NoCollision"));*/
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
	//LeavesMesh->SetRenderCustomDepth(false);
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

void APlant::MakeGold()
{
	if (ItemComponent->bGold)
		return;
	
	if (GoldMaterial)
	{
		ItemComponent->Mesh->SetMaterial(0, GoldMaterial);
	}
	if (GoldMaterial2)
	{
		ItemComponent->Mesh->SetMaterial(1, GoldMaterial2);
	}
	if (GoldMaterial3)
	{
		ItemComponent->Mesh->SetMaterial(2, GoldMaterial3);
	}
	ItemComponent->CropValue *= GoldMultiplier;
	ItemComponent->bGold = true;
}

void APlant::Server_ToggleGold_Implementation()
{
	Multi_ToggleGold();
}

void APlant::Multi_ToggleGold_Implementation()
{
	int32 X = rand() % 100;
	if (X < ChanceOfGold)
	{
		MakeGold();
	}
}
