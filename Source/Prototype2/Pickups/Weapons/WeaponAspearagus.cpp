

#include "WeaponAspearagus.h"

#include "AspearagusProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"

UWeaponAspearagus::UWeaponAspearagus()
{
	static ConstructorHelpers::FClassFinder<AAspearagusProjectile> AspearagusPrefab(TEXT("/Game/Blueprints/Pickups/BP_AspearagusProjectile"));
	if (IsValid(AspearagusPrefab.Class))
	{
		Prefab = AspearagusPrefab.Class;
	}
}

void UWeaponAspearagus::Client_ChargeAttack(APrototype2Character* _Player)
{
	_Player->SetPlayerAimingMovement(true);
}

void UWeaponAspearagus::ChargeAttack(APrototype2Character* _Player)
{
}

void UWeaponAspearagus::Client_ChargeAttackCancelled(APrototype2Character* _Player)
{
}

void UWeaponAspearagus::ChargeAttackCancelled(APrototype2Character* _Player)
{
}

void UWeaponAspearagus::Client_ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
	_Player->SetPlayerAimingMovement(false);
}

void UWeaponAspearagus::ReleaseAttack(bool _bIsFullCharge, APrototype2Character* _Player)
{
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

void UWeaponAspearagus::Client_ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player,
	float _AttackChargeAmount)
{
}

void UWeaponAspearagus::ExecuteAttack(float _AttackSphereRadius, APrototype2Character* _Player,  float _AttackChargeAmount, bool bIsSprinting)
{
	Super::ExecuteAttack(_AttackSphereRadius, _Player, _AttackChargeAmount, bIsSprinting);

	UE_LOG(LogTemp, Warning, TEXT("Spawn Aspearagus Projectile"));
	
	Server_SpawnProjectile(_Player, _AttackSphereRadius, _AttackChargeAmount);
	
	// make UI pop out
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
	_Player->PlayWeaponSound(_Player->CurrentWeaponSeedData->WeaponData->AttackAudio);
	
	// Reset all attack variables
	_Player->ResetAttack();
}

void UWeaponAspearagus::UpdateAOEIndicator(APrototype2Character* _Player, float _AttackChargeAmount)
{
	_Player->AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	float AttackSphereRadius = _Player->CurrentWeaponSeedData->WeaponData->BaseAttackRadius;// + _AttackChargeAmount * _Player->CurrentWeaponSeedData->WeaponData->AOEMultiplier;	
	
	FVector DownVector = {_Player->GetActorLocation().X, _Player->GetActorLocation().Y, _Player->GetMesh()->GetComponentLocation().Z};
	DownVector += _Player->GetActorForwardVector() * 10000.0f;
	
	_Player->AttackAreaIndicatorMesh->SetWorldLocation(DownVector);
	_Player->AttackAreaIndicatorMesh->SetWorldRotation(_Player->GetActorRotation());
	_Player->AttackAreaIndicatorMesh->SetRelativeScale3D({10000.0f, AttackSphereRadius,_AttackChargeAmount * 30.0f});// Magic number just to increase the height of the aoe indicator

}

void UWeaponAspearagus::Multi_SpawnProjectile_Implementation(APrototype2Character* _Player, float _AttackSphereRadius, float _AttackChargeAmount)
{
}

void UWeaponAspearagus::Server_SpawnProjectile_Implementation(APrototype2Character* _Player, float _AttackSphereRadius, float _AttackChargeAmount)
{
	if (!_Player)
		return;

	FTransform ProjectileTransform = _Player->GetTransform();
	ProjectileTransform.SetScale3D(_Player->WeaponMesh->GetComponentScale());
	AAspearagusProjectile* NewAspearagusProjectile = GetWorld()->SpawnActor<AAspearagusProjectile>(Prefab, ProjectileTransform);
	if (NewAspearagusProjectile)
	{
		NewAspearagusProjectile->SetReplicates(true);
		NewAspearagusProjectile->Server_InitializeProjectile(_Player, _Player->CurrentWeaponSeedData->BabyMesh,
												2800.0f, 3.0f, _AttackSphereRadius, _AttackChargeAmount);
	}
}
