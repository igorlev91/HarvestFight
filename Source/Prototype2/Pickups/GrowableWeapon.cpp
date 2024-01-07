
#include "GrowableWeapon.h"
#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"

AGrowableWeapon::AGrowableWeapon()
{
	bReplicates = true;

	InterfaceType = EInterfaceType::Default;
}

void AGrowableWeapon::Interact(APrototype2Character* _Player)
{
	// attach the mesh to the player
}

/*
void AGrowableWeapon::OnDisplayInteractText(UWidget_PlayerHUD* InvokingWidget, APrototype2Character* Owner,
	int _PlayerID)
{
	if (!Owner->HeldItem)
	{
		InvokingWidget->SetHUDInteractText("Pick Up");
	}
}*/
