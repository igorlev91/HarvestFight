

#include "WeaponAspearagus.h"

#include "AspearagusProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"

void UWeaponAspearagus::ChargeAttack(APrototype2Character* _Player)
{
	Super::ChargeAttack(_Player);

	_Player->SetPlayerAimingMovement(true);
}

void UWeaponAspearagus::ChargeAttackCancelled(APrototype2Character* _Player)
{
	Super::ChargeAttackCancelled(_Player);
	_Player->SetPlayerAimingMovement(false);
}

void UWeaponAspearagus::ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
	Super::ReleaseAttack(_bIsFullCharge, _Player);

	_Player->SetPlayerAimingMovement(false);
	
	if (_bIsFullCharge)
	{
		if (_Player->AnimationData->FullChargePunchingAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->FullChargeAspearagusAttack);
		}
	}
	else
	{
		if (_Player->AnimationData->NormalPunchingAttack)
		{
			_Player->PlayNetworkMontage(_Player->AnimationData->NormalAspearagusAttack);
		}
	}
}

void UWeaponAspearagus::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player, FVector _CachedActorLocation, FVector _CachedForwardVector)
{
	Super::ExecuteAttack(_AttackSphereRadius, _Player, _CachedActorLocation, _CachedForwardVector);
	
	Server_SpawnProjectile(_Player, _AttackSphereRadius);
	
	// make UI pop out
	//_Player->OnExecuteAttackDelegate.Broadcast();
	BroadcastAttackToHUD(_Player);

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

void UWeaponAspearagus::UpdateAOEIndicator(APrototype2Character* _Player)
{
	Super::UpdateAOEIndicator(_Player);
		
	_Player->AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	float AttackSphereRadius = _Player->CurrentWeaponSeedData->WeaponData->BaseAttackRadius + _Player->AttackChargeAmount * _Player->CurrentWeaponSeedData->WeaponData->AOEMultiplier;	
	
	FVector DownVector = {_Player->GetActorLocation().X, _Player->GetActorLocation().Y, _Player->GetMesh()->GetComponentLocation().Z};
	DownVector += _Player->GetActorForwardVector() * 10000.0f;
	
	_Player->AttackAreaIndicatorMesh->SetWorldLocation(DownVector);
	_Player->AttackAreaIndicatorMesh->SetWorldRotation(_Player->GetActorRotation());
	_Player->AttackAreaIndicatorMesh->SetRelativeScale3D({10000.0f, AttackSphereRadius,_Player->AttackChargeAmount * 30.0f});// Magic number just to increase the height of the aoe indicator

}

void UWeaponAspearagus::Multi_SpawnProjectile_Implementation(APrototype2Character* _Player, float _AttackSphereRadius)
{
	if (!_Player)
		return;
	
	// Spawn projectile
	FTransform ProjectileTransform = _Player->GetTransform();
	ProjectileTransform.SetScale3D(_Player->WeaponMesh->GetComponentScale());
	AAspearagusProjectile* NewAspearagusProjectile = GetWorld()->SpawnActorDeferred<AAspearagusProjectile>(AAspearagusProjectile::StaticClass(),ProjectileTransform, _Player);
	if (NewAspearagusProjectile)
	{
		NewAspearagusProjectile->InitializeProjectile(_Player, _Player->CurrentWeaponSeedData->BabyMesh,
												4000.0f, 3.0f, _AttackSphereRadius);
		NewAspearagusProjectile->SetOwner(_Player);
		UGameplayStatics::FinishSpawningActor(NewAspearagusProjectile, ProjectileTransform);
		NewAspearagusProjectile->SetReplicates(true);
		NewAspearagusProjectile->SetReplicateMovement(true);
	}
}

void UWeaponAspearagus::Server_SpawnProjectile_Implementation(APrototype2Character* _Player, float _AttackSphereRadius)
{
	Multi_SpawnProjectile(_Player, _AttackSphereRadius);
}
