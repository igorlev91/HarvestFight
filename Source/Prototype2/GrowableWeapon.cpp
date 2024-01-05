#include "GrowableWeapon.h"
#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2Character.h"

AGrowableWeapon::AGrowableWeapon()
{
	bReplicates = true;
	//Weapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));
	//Weapon->Mesh->SetSimulatePhysics(false);
	//Weapon->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Weapon->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	InterfaceType = EInterfaceType::Default;
}

/*
void AGrowableWeapon::BeginPlay()
{
	Super::BeginPlay();
}
*/
void AGrowableWeapon::Interact(APrototype2Character* player)
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
