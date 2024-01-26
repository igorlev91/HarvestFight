

#include "WeaponHoneyStick.h"

#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/GrowSpot.h"

void UWeaponHoneyStick::ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
	Super::ReleaseAttack(_bIsFullCharge, _Player);
	
	if (_bIsFullCharge)
	{
		if (_Player->AnimationData->FullChargePunchingAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->FullChargeHoneyStickAttack);
		}
	}
	else
	{
		if (_Player->AnimationData->NormalPunchingAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->NormalHoneyStickAttack);
		}
	}
}

void UWeaponHoneyStick::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector)
{
	_CachedActorLocation = _Player->GetActorLocation();
	_CachedForwardVector = _Player->GetActorForwardVector();
	
	// Scale the actual area of effect to be larger than what the indicator shows 
	_AttackSphereRadius *= _Player->CurrentWeaponSeedData->WeaponData->ScaleOfAOELargerThanIndicator;
	
	Super::ExecuteAttack(_AttackSphereRadius, _Player, _CachedActorLocation, _CachedForwardVector);
	
	// create a collision sphere
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(_AttackSphereRadius);

	// Trigger the VFX in blueprint
	const FVector DownVector = {_Player->GetActorLocation().X, _Player->GetActorLocation().Y, _Player->GetMesh()->GetComponentLocation().Z};
	_Player->TriggerAttackVFX(DownVector, _AttackSphereRadius, _Player->AttackChargeAmount);	

	// create tarray for catching hit results
	TArray<FHitResult> OutHits;

	// Set the sweep to stationary
	const FVector SweepStart = _Player->GetActorLocation();
	const FVector SweepEnd = _Player->GetActorLocation();
	
	// check if something got hit in the sweep
	const bool bHasHitResult = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Pawn, CollisionSphere);

	if (bHasHitResult)
	{
		// Check if the hits were players or sell bin
		for (auto HitResult : OutHits)
		{
			if (auto HitPlayerCast = Cast<APrototype2Character>(HitResult.GetActor()))
			{
				if (HitPlayerCast != _Player)
				{
					HitPlayerCast->GetHit(_Player->AttackChargeAmount, _Player->GetActorLocation(), _Player->CurrentWeaponSeedData->WeaponData);
				}
			}
			else if (auto HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
			{
				HitSellBinCast->GetHit(_Player->AttackChargeAmount, _Player->MaxAttackCharge, _Player->GetActorLocation());
			}

			if (auto GrowSpot = Cast<AGrowSpot>(HitResult.GetActor()))
			{
				GrowSpot->DegradeConcrete();
			}
		}
	}
	
	// make UI pop out
	//_Player->OnExecuteAttackDelegate.Broadcast();
	Client_BroadcastAttackToHUD(_Player);

	_Player->WeaponCurrentDurability--;
	_Player->PlayerHUDRef->SetWeaponDurability(_Player->WeaponCurrentDurability);
	if (_Player->WeaponCurrentDurability <= 0)
	{
		_Player->DropWeapon();

		//AttackTrail_NiagaraComponent->Deactivate();
		_Player->DeActivateParticleSystemFromEnum(EParticleSystems::AttackTrail);
	}

	// Play attack audio
	_Player->PlaySoundAtLocation(_Player->GetActorLocation(), _Player->CurrentWeaponSeedData->WeaponData->AttackAudio);

	// Reset all attack variables
	_Player->ResetAttack();
}

void UWeaponHoneyStick::UpdateAOEIndicator(APrototype2Character* _Player)
{
	Super::UpdateAOEIndicator(_Player);
	
	_Player->AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	float AttackSphereRadius = _Player->CurrentWeaponSeedData->WeaponData->BaseAttackRadius + _Player->AttackChargeAmount * _Player->CurrentWeaponSeedData->WeaponData->AOEMultiplier;	
	FVector DownVector = {_Player->GetActorLocation().X, _Player->GetActorLocation().Y, _Player->GetMesh()->GetComponentLocation().Z};

	_Player->AttackAreaIndicatorMesh->SetWorldLocation(DownVector);
	_Player->AttackAreaIndicatorMesh->SetRelativeScale3D({AttackSphereRadius,AttackSphereRadius,_Player->AttackChargeAmount * 30.0f});// Magic number just to increase the height of the aoe indicator

}
