


#include "Plant.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/DataAssets/SeedData.h"

APlant::APlant()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	InterfaceType = EInterfaceType::Default;

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

	WiltingDelayTimer = 0;
	ItemComponent->Interact(_Player, this);
	Multi_SetWilt(false);


}

void APlant::HoldInteract(APrototype2Character* _Player)
{
}

void APlant::ClientInteract(APrototype2Character* _Player)
{
	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);
	
	SSComponent->Boing();
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

	Multi_Wilt(DeltaSeconds);
}

void APlant::Server_Drop()
{
	Super::Server_Drop();
	Multi_SetWilt(true);
}

void APlant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlant, WiltingDelayTimer)
	DOREPLIFETIME(APlant, WiltTime)
	DOREPLIFETIME(APlant, bShouldWilt)
}

void APlant::Multi_Wilt(float DeltaTime)
{
	if (!bShouldWilt)
		return;
	
	WiltingDelayTimer += DeltaTime;
	
	if (WiltingDelayTimer < WiltingWaitTime)
	{
		return;
	}
	
	WiltTime -= DeltaTime;

	if (WiltTime <= 0)
	{
		this->Destroy();
	}
}

void APlant::Multi_ScalePlant()
{
	ItemComponent->Mesh->SetWorldScale3D(SeedData->BabyScale);
}

void APlant::Multi_SetWilt(bool _bShouldWilt)
{
	bShouldWilt = _bShouldWilt;
}
