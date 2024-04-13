

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
		if (_Player->AnimationData->FullChargeHoneyStickAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->FullChargeHoneyStickAttack);
		}
	}
	else
	{
		if (_Player->AnimationData->NormalHoneyStickAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->NormalHoneyStickAttack);
		}
	}
}

void UWeaponHoneyStick::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player,  float _AttackChargeAmount, bool bIsSprinting)
{
	if (!IsValid(_Player))
		return;
	
	// Scale the actual area of effect to be larger than what the indicator shows 
	_AttackSphereRadius *= _Player->CurrentWeaponSeedData->WeaponData->ScaleOfAOELargerThanIndicator;
	
	Super::ExecuteAttack(_AttackSphereRadius, _Player, _AttackChargeAmount, bIsSprinting);
	
	// create a collision sphere
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(_AttackSphereRadius);

	// Trigger the VFX in blueprint
	const FVector DownVector = {_Player->GetActorLocation().X, _Player->GetActorLocation().Y, _Player->GetMesh()->GetComponentLocation().Z};
	_Player->TriggerAttackVFX(DownVector, _AttackSphereRadius, _AttackChargeAmount);	

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
					//HitPlayerCast->GetHit(_AttackChargeAmount, _Player->GetActorLocation(), _Player->CurrentWeaponSeedData->WeaponData);
					//CheckIfCrownHit(_Player, HitPlayerCast);
					_Player->Server_HitPlayer(HitPlayerCast, _AttackChargeAmount, _Player->GetActorLocation(), _Player->CurrentWeaponSeedData->WeaponData);
					_Player->Server_CheckIfCrownHit(HitPlayerCast);
					if (HitPlayerCast->GetHasCrown())
					{
						_Player->PlaySoundAtLocation(HitPlayerCast->GetActorLocation(), HitPlayerCast->SellCue);
					}
				}
			}
			else if (auto HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
			{
				//HitSellBinCast->GetHit(_AttackChargeAmount, _Player->MaxAttackCharge, _Player->GetActorLocation());
				_Player->Server_HitWinterSellBin(HitSellBinCast, _AttackChargeAmount, _Player->MaxAttackCharge, _Player->GetActorLocation());
			}

			if (auto GrowSpot = Cast<AGrowSpot>(HitResult.GetActor()))
			{
				//if (GrowSpot->DegradeConcrete())
				//{
				//	_Player->PlaySoundAtLocation(_Player->GetActorLocation(), _Player->HitConcreteCue);
				//}
				_Player->Server_HitConcrete(GrowSpot);
			}
		}
	}
	
	// make UI pop out
	_Player->OnExecuteAttackDelegate.Broadcast();
	//Client_BroadcastAttackToHUD(_Player);

	_Player->WeaponCurrentDurability--;
	_Player->PlayerHUDRef->SetWeaponDurability(_Player->WeaponCurrentDurability);
	
	if (_Player->WeaponCurrentDurability <= 0)
	{
		_Player->DropWeapon();
		
		_Player->DeActivateParticleSystemFromEnum(EParticleSystems::AttackTrail);
	}

	// Play attack audio
	_Player->PlayWeaponSound(_Player->CurrentWeaponSeedData->WeaponData->AttackAudio);
	
	// Reset all attack variables
	_Player->ResetAttack();
}

void UWeaponHoneyStick::UpdateAOEIndicator(APrototype2Character* _Player, float _AttackChargeAmount)
{
	if (!IsValid(_Player))
		return;
	
	Super::UpdateAOEIndicator(_Player, _AttackChargeAmount);
	
	_Player->AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	float AttackSphereRadius = _Player->CurrentWeaponSeedData->WeaponData->BaseAttackRadius + _AttackChargeAmount * _Player->CurrentWeaponSeedData->WeaponData->AOEMultiplier;	
	FVector DownVector = {_Player->GetActorLocation().X, _Player->GetActorLocation().Y, _Player->GetMesh()->GetComponentLocation().Z};

	_Player->AttackAreaIndicatorMesh->SetWorldLocation(DownVector);
	_Player->AttackAreaIndicatorMesh->SetRelativeScale3D({AttackSphereRadius,AttackSphereRadius,_AttackChargeAmount * 30.0f});// Magic number just to increase the height of the aoe indicator

}
