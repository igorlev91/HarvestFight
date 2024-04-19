#include "Weapon.h"
#include "Prototype2/InteractInterface.h"
#include "PickUpItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

UWeapon::UWeapon()
{
	SetIsReplicatedByDefault(true);
}

void UWeapon::ChargeAttackCancelled(APrototype2Character* _Player)
{
	if (!_Player)
		return;
	
	_Player->bAllowMovementFromInput = true;
}

void UWeapon::Client_ReleaseAttack_Implementation(bool _bIsFullCharge, APrototype2Character* _Player)
{
}

void UWeapon::ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
	if (!IsValid(_Player))
		return;
	
	_Player->bAllowMovementFromInput = false;
	
	Client_ReleaseAttack(_bIsFullCharge, _Player);
	
	// if player isn't standing still, launch them forward
	if (_Player->GetVelocity().Size() == 0.0f)
		return;

	if (_bIsFullCharge)
	{
		_Player->LungeAttack(_Player->GetActorForwardVector() * _Player->CurrentWeaponSeedData->WeaponData->LaunchVelocityMultiplier);
	}
}


void UWeapon::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, float _AttackChargeAmount, bool _bSprinting)
{
	if (!IsValid(_Player))
		return;
	
	_Player->WeaponFlashTimer = _Player->WeaponFlashDuration;
	_Player->bShouldWeaponFlashRed = true;
	_Player->bAllowMovementFromInput = true;
	_Player->bIsFollowingThroughAttack = false;
	_Player->Grunt();

}

void UWeapon::CheckIfCrownHit(APrototype2Character* _Attacker, APrototype2Character* _Victim)
{
	if (_Victim->GetHasCrown())
	{
		int32 PointsForHit = static_cast<int32>(_Attacker->AttackChargeAmount);
		if (PointsForHit < 1)
		{
			PointsForHit = 1;
		}
		_Attacker->PlayerStateRef->AddCoins(PointsForHit);
		_Victim->PlaySoundAtLocation(_Victim->GetActorLocation(), _Victim->SellCue);
	}
}

void UWeapon::Client_SetFollowingThroughToFalse_Implementation(APrototype2Character* _Player)
{
	if (IsValid(_Player))
		_Player->bIsFollowingThroughAttack = false;
	
}

void UWeapon::Client_BroadcastAttackToHUD_Implementation(APrototype2Character* _Player)
{
	if (IsValid(_Player))
		_Player->OnExecuteAttackDelegate.Broadcast();
}


