

#include "AspearagusProjectile.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"


// Sets default values
AAspearagusProjectile::AAspearagusProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AspearagusMesh = CreateDefaultSubobject<UStaticMeshComponent>("AspearagusMesh");
	SetRootComponent(AspearagusMesh);
}

//void AAspearagusProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(AAspearagusProjectile, AspearagusMesh);
//}

// Called when the game starts or when spawned
void AAspearagusProjectile::BeginPlay()
{
	Super::BeginPlay();
	AspearagusMesh->SetSimulatePhysics(false);
	AspearagusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AspearagusMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

// Called every frame
void AAspearagusProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeathTimer >= 0.0f)
	{
		DeathTimer -= DeltaTime;
		SetActorLocation(GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime);
	}
	else
	{
		Destroy();
	}
	
	if (OwningPlayer)
	{
		CheckForHitPlayers();
	}
	
}

void AAspearagusProjectile::CheckForHitPlayers()
{
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
		for (auto& HitResult : OutHits)
		{
			if (auto* HitPlayerCast = Cast<APrototype2Character>(HitResult.GetActor()))
			{
				if (HitPlayerCast != OwningPlayer)
				{
					HitPlayerCast->GetHit(ChargeAmount, GetActorLocation(), OwningPlayer->CurrentWeaponData);
				}
			}
			else if (auto* HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
			{
				HitSellBinCast->GetHit(1, OwningPlayer->MaxAttackCharge, GetActorLocation());
			}
		}
	}
}

void AAspearagusProjectile::InitializeProjectile(APrototype2Character* _Player, UStaticMesh* _Mesh, float _Speed,
	float _LifeTime, float _AttackSphereRadius)
{
	OwningPlayer = _Player;
	AspearagusMesh->SetStaticMesh(_Mesh);
	Speed = _Speed;
	DeathTimer = _LifeTime;
	AttackSphereRadius = _AttackSphereRadius;
	ChargeAmount = _Player->AttackChargeAmount;
}

