// Fill out your copyright notice in the Description page of Project Settings.


#include "Seed.h"

#include "Prototype2Character.h"

ASeed::ASeed()
{
	bReplicates = true;
	InterfaceType = EInterfaceType::Default;
}

void ASeed::Interact(APrototype2Character* player)
{
	ItemComponent->Interact(player, this);
	
	player->EnableStencil(false);
	if (player->PlayerHUDRef)
	{
		player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);
}

void ASeed::ClientInteract(APrototype2Character* player)
{
	IInteractInterface::ClientInteract(player);

	player->UpdateDecalDirection(true, false);
}

void ASeed::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	if (!owner->HeldItem || owner->HeldItem != this)
	{
		_invokingWiget->SetHUDInteractText("Pick Up");

		owner->EnableStencil(true);
	}
}

void ASeed::Grow()
{
}
