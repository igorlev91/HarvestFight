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
	InterfaceType = EInterfaceType::Weapon;
	SetIsReplicatedByDefault(true);
}

void UWeapon::Interact(APrototype2Character* _Player)
{
}

void UWeapon::HoldInteract(APrototype2Character* _Player)
{
}

void UWeapon::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
}

bool UWeapon::IsInteractable(APrototype2PlayerState* _Player)
{
	return false;
}

void UWeapon::ChargeAttack(APrototype2Character* _Player)
{
}

void UWeapon::ChargeAttackCancelled(APrototype2Character* _Player)
{
}

void UWeapon::ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
}

void UWeapon::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector)
{
}

void UWeapon::UpdateAOEIndicator(APrototype2Character* _Player)
{
}

void UWeapon::BroadcastAttackToHUD_Implementation(APrototype2Character* _Player)
{
	_Player->OnExecuteAttackDelegate.Broadcast();
}


