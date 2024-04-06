

#include "AspearagusProjectile.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/Gameplay/SkyAlter.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"


// Sets default values
AAspearagusProjectile::AAspearagusProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 2500.0f;
	ProjectileMovement->MaxSpeed = 2500.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bInterpMovement = true;
	ProjectileMovement->bRotationFollowsVelocity = true;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(SphereCollision);
	SphereCollision->InitSphereRadius(15.0f);
	SphereCollision->SetNotifyRigidBodyCollision(true);
	SphereCollision->OnComponentHit.AddDynamic(this, &AAspearagusProjectile::OnHit);
	SphereCollision->SetCollisionProfileName(FName("BlockAll"));

	AspearagusMesh = CreateDefaultSubobject<UStaticMeshComponent>("AspearagusMesh");
	AspearagusMesh->SetCollisionProfileName(FName("NoCollision"));
	AspearagusMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundAspearagusMesh(TEXT("/Game/PickUps/Weapons/SM_Asparagus_Weapon"));
	if (IsValid(FoundAspearagusMesh.Object))
	{
		AspearagusMesh->SetStaticMesh(FoundAspearagusMesh.Object);
	}
}

// Called when the game starts or when spawned
void AAspearagusProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Ignore all spawned Pickup Items
	{
		TArray<AActor*> FoundItems{};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickUpItem::StaticClass(), FoundItems);
		for(auto FoundItem : FoundItems)
			SphereCollision->IgnoreActorWhenMoving(FoundItem, true);
	}
}

// Called every frame
void AAspearagusProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAspearagusProjectile::Destroyed()
{
	Super::Destroyed();
	
}

void AAspearagusProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor))
		return;

	if (!HasAuthority())
		return;
	
	if (APrototype2Character* HitPlayerCast = Cast<APrototype2Character>(OtherActor))
	{
		if (HitPlayerCast != OwningPlayer)
		{
			Destroy();
			HitPlayerCast->GetHit(ChargeAmount, GetActorLocation(), OwningPlayer->CurrentWeaponSeedData->WeaponData);

			if (HitPlayerCast->GetHasCrown())
			{
				int32 PointsForHit = static_cast<int32>(OwningPlayer->AttackChargeAmount);
				if (PointsForHit < 1)
				{
					PointsForHit = 1;
				}
				OwningPlayer->PlayerStateRef->AddCoins(PointsForHit);
				HitPlayerCast->PlaySoundAtLocation(HitPlayerCast->GetActorLocation(), HitPlayerCast->SellCue);
			}
		}
		return;
	}

	if (auto PickupItem = Cast<APickUpItem>(OtherActor))
		return;

	if (auto HitSellBinCast = Cast<ASellBin_Winter>(OtherActor))
	{
		HitSellBinCast->GetHit(ChargeAmount, OwningPlayer->MaxAttackCharge, GetActorLocation());
	}
	
	Destroy();
}

void AAspearagusProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAspearagusProjectile, AttackSphereRadius);
	DOREPLIFETIME(AAspearagusProjectile, OwningPlayer);
	DOREPLIFETIME(AAspearagusProjectile, AspearagusMesh);
	DOREPLIFETIME(AAspearagusProjectile, ChargeAmount);
}

void AAspearagusProjectile::OnRep_Initialization()
{
	if (IsValid(OwningPlayer))
	{
		SphereCollision->IgnoreActorWhenMoving(OwningPlayer, true);
		OwningPlayer->MoveIgnoreActorAdd(this);
	}
}

void AAspearagusProjectile::Server_InitializeProjectile_Implementation(APrototype2Character* _Player,
                                                                       UStaticMesh* _Mesh, float _Speed, float _LifeTime, float _AttackSphereRadius, float _AttackChargeAmount)
{
	OwningPlayer = _Player;
	AspearagusMesh->SetStaticMesh(_Mesh);
	DeathTimer = _LifeTime;
	AttackSphereRadius = _AttackSphereRadius * _Player->CurrentWeaponSeedData->WeaponData->ScaleOfAOELargerThanIndicator;
	ChargeAmount = _AttackChargeAmount;

	if (IsValid(OwningPlayer))
	{
		SphereCollision->IgnoreActorWhenMoving(OwningPlayer, true);
		OwningPlayer->MoveIgnoreActorAdd(this);
	}
}

