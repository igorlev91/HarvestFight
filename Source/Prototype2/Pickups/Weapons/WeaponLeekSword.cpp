

#include "WeaponLeekSword.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/DataAssets/SeedData.h"

void UWeaponLeekSword::ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
	Super::ReleaseAttack(_bIsFullCharge, _Player);
	
	if (_bIsFullCharge)
	{
		if (_Player->AnimationData->FullChargePunchingAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->FullChargeLeekAttack);
		}
	}
	else
	{
		if (_Player->AnimationData->NormalPunchingAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->NormalLeekAttack);
		}
	}
}

void UWeaponLeekSword::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player)
{
	Super::ExecuteAttack(_AttackSphereRadius, _Player);
	
	// Get a vector infront of the character for the attack sphere to spawn at
	const FVector InFrontOfPlayer = _Player->GetActorLocation() + (_Player->GetActorForwardVector() * _AttackSphereRadius) + (_Player->GetActorForwardVector() * _Player->CurrentWeaponSeedData->WeaponData->WeaponReach);
	
	// create a collision sphere
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(_AttackSphereRadius);

	// Trigger the VFX in blueprint
	const FVector DownVector = {InFrontOfPlayer.X, InFrontOfPlayer.Y, _Player->GetMesh()->GetComponentLocation().Z};
	_Player->TriggerAttackVFX(DownVector, _AttackSphereRadius, _Player->AttackChargeAmount);	

	// create tarray for catching hit results
	TArray<FHitResult> OutHits;

	// Set the sweep to stationary
	const FVector SweepStart = InFrontOfPlayer;
	const FVector SweepEnd = InFrontOfPlayer;
	
	// check if something got hit in the sweep
	const bool bHasHitResult = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Pawn, CollisionSphere);

	// For holding if anyone was hit to degrade weapon later
	bool bIsOtherPlayerHit = false;
	
	if (bHasHitResult)
	{
		// Check if the hits were players or sell bin
		for (auto& HitResult : OutHits)
		{
			if (auto* HitPlayerCast = Cast<APrototype2Character>(HitResult.GetActor()))
			{
				if (HitPlayerCast != _Player)
				{
					HitPlayerCast->GetHit(_Player->AttackChargeAmount, _Player->GetActorLocation(), _Player->CurrentWeaponSeedData->WeaponData);

					bIsOtherPlayerHit = true;
				}
			}
			else if (auto* HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
			{
				HitSellBinCast->GetHit(_Player->AttackChargeAmount, _Player->MaxAttackCharge, _Player->GetActorLocation());
			}
		}
	}

	// Lower weapon durability
	//if (bIsOtherPlayerHit)
	//{
		_Player->OnExecuteAttackDelegate.Broadcast();
	
		_Player->WeaponCurrentDurability--;
		_Player->PlayerHUDRef->SetWeaponDurability(_Player->WeaponCurrentDurability);
		
		if (_Player->WeaponCurrentDurability <= 0)
		{
			_Player->DropWeapon();

			//AttackTrail_NiagaraComponent->Deactivate();
			_Player->DeActivateParticleSystemFromEnum(EParticleSystems::AttackTrail);
		}
	//}
	// Play attack audio
	_Player->PlaySoundAtLocation(_Player->GetActorLocation(), _Player->CurrentWeaponSeedData->WeaponData->AttackAudio);

	// Reset all attack variables
	_Player->ResetAttack();
}

void UWeaponLeekSword::UpdateAOEIndicator(APrototype2Character* _Player)
{
	Super::UpdateAOEIndicator(_Player);
	
	_Player->AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	float AttackSphereRadius = _Player->CurrentWeaponSeedData->WeaponData->BaseAttackRadius + _Player->AttackChargeAmount * _Player->CurrentWeaponSeedData->WeaponData->AOEMultiplier;	
	FVector InFrontOfPlayer = _Player->GetActorLocation() + (_Player->GetActorForwardVector() * AttackSphereRadius) + (_Player->GetActorForwardVector() * _Player->CurrentWeaponSeedData->WeaponData->WeaponReach);
	
	FVector DownVector = {InFrontOfPlayer.X, InFrontOfPlayer.Y, _Player->GetMesh()->GetComponentLocation().Z};

	_Player->AttackAreaIndicatorMesh->SetWorldLocation(DownVector);
	_Player->AttackAreaIndicatorMesh->SetRelativeScale3D({AttackSphereRadius,AttackSphereRadius,_Player->AttackChargeAmount * 30.0f});// Magic number just to increase the height of the aoe indicator

}
