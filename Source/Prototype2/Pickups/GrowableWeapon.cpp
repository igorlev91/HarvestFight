


#include "GrowableWeapon.h"
#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/VFX/SquashAndStretch.h"

AGrowableWeapon::AGrowableWeapon()
{
	bReplicates = true;

	InterfaceType = EInterfaceType::Default;
}

void AGrowableWeapon::Interact(APrototype2Character* _Player)
{
	SSComponent->Disable();
}
