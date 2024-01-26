

#include "AspearagusProjectile.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"


// Sets default values
AAspearagusProjectile::AAspearagusProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	AspearagusMesh = CreateDefaultSubobject<UStaticMeshComponent>("AspearagusMesh");
	SetRootComponent(AspearagusMesh);
}

// Called when the game starts or when spawned
void AAspearagusProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	AspearagusMesh->SetVisibility(true);
	AspearagusMesh->SetSimulatePhysics(false);
	AspearagusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AspearagusMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

// Called every frame
void AAspearagusProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (DeathTimer > 0)
	{
		DeathTimer -= DeltaTime;

		if (!bHitSomething)
		{
			SetActorLocation(GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime);
			CheckForHitObstacle();
			CheckForHitPlayers();
		}

		if (DeathTimer <= 0)
		{
			Destroy();
		}
	}
}

void AAspearagusProjectile::CheckForHitPlayers()
{
	if (!OwningPlayer)
		return;

	if (bHitSomething)
		return;
	
	// create a collision sphere
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(AttackSphereRadius);

	// create tarray for catching hit results
	TArray<FHitResult> OutHits;

	// Set the sweep to stationary
	const FVector SweepStart = GetActorLocation();
	const FVector SweepEnd = GetActorLocation();
	
	// check if something got hit in the sweep
	const bool bHasHitResult = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Pawn, CollisionSphere);

	if (bHasHitResult)
	{

		// Check if the hits were players or sell bin
		for (auto HitResult : OutHits)
		{
			if (auto HitPlayerCast = Cast<APrototype2Character>(HitResult.GetActor()))
			{
				if (HitPlayerCast != OwningPlayer)
				{
					HitPlayerCast->GetHit(ChargeAmount, GetActorLocation(), OwningPlayer->CurrentWeaponSeedData->WeaponData);
					Destroy();
				}
			}
			else if (auto HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
			{
				HitSellBinCast->GetHit(ChargeAmount, OwningPlayer->MaxAttackCharge, GetActorLocation());
				Destroy();
			}
		}
	}
}

bool AAspearagusProjectile::CheckForHitObstacle()
{
	if (bHitSomething)
		return true;
	
	// create a collision sphere
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(30.0f);

	// create tarray for catching hit results
	TArray<FHitResult> OutHits;

	
	// Set the sweep to stationary
	const FVector SweepStart = AspearagusMesh->GetSocketLocation("AspearagusHead");
	const FVector SweepEnd = SweepStart;
	
	// check if something got hit in the sweep
	if (GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Visibility, CollisionSphere))
	{
		for (auto HitResult : OutHits)
		{
			if (auto HitPlayerCast = Cast<APrototype2Character>(HitResult.GetActor()))
			{
				if (HitPlayerCast != OwningPlayer)
				{
					bHitSomething = true;
				}
			}
			else if (auto HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
			{
				HitSellBinCast->GetHit(ChargeAmount, OwningPlayer->MaxAttackCharge, GetActorLocation());
				Destroy();
			}
			else if (!Cast<AGrowSpot>(HitResult.GetActor()))
			{
				bHitSomething = true;
			}
		}
	}
	return bHitSomething;
}

void AAspearagusProjectile::InitializeProjectile(APrototype2Character* _Player, UStaticMesh* _Mesh, float _Speed,
                                                                       float _LifeTime, float _AttackSphereRadius)
{
	OwningPlayer = _Player;
	AspearagusMesh->SetStaticMesh(_Mesh);
	Speed = _Speed;
	DeathTimer = _LifeTime;
	AttackSphereRadius = _AttackSphereRadius * OwningPlayer->CurrentWeaponSeedData->WeaponData->ScaleOfAOELargerThanIndicator;
	ChargeAmount = _Player->AttackChargeAmount;
}

