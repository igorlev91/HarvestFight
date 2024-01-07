#include "Weapon.h"
#include "Prototype2/InteractInterface.h"
#include "PickUpItem.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"

UWeapon::UWeapon()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	InterfaceType = EInterfaceType::Weapon;
}

void UWeapon::Interact(APrototype2Character* _Player)
{
}

void UWeapon::OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID)
{
}

bool UWeapon::IsInteractable(APrototype2PlayerState* _Player)
{
	return false;
}


