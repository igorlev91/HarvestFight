#include "Weapon.h"
#include "Prototype2/InteractInterface.h"
#include "PickUpItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/VFX/SquashAndStretch.h"

UWeapon::UWeapon()
{
	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	SetIsReplicatedByDefault(true);
}

void UWeapon::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector)
{
	_Player->WeaponFlashTimer = _Player->WeaponFlashDuration;
	_Player->bShouldWeaponFlashRed = true;
}

void UWeapon::Client_BroadcastAttackToHUD_Implementation(APrototype2Character* _Player)
{
	if (_Player)
		_Player->OnExecuteAttackDelegate.Broadcast();
}


