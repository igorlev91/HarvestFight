


#include "SkyAlterAttack.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Pickups/Plant.h"


// Sets default values
ASkyAlterAttack::ASkyAlterAttack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
}

// Called when the game starts or when spawned
void ASkyAlterAttack::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->OnComponentHit.AddDynamic(this, &ASkyAlterAttack::OnPlayerTouchAltar);
	BoxComponent1->OnComponentHit.AddDynamic(this, &ASkyAlterAttack::OnPlayerTouchAltar);
	BoxComponent2->OnComponentHit.AddDynamic(this, &ASkyAlterAttack::OnPlayerTouchAltar);
	BoxComponent3->OnComponentHit.AddDynamic(this, &ASkyAlterAttack::OnPlayerTouchAltar);
}

void ASkyAlterAttack::OnPlayerTouchAltar(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent*
	OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UKismetSystemLibrary::PrintString(GetWorld(), "Hit");
	if (!OtherActor->HasNetOwner())
		return;
	
	if (!IsValid(OtherActor))
		return;
	APrototype2Character* SomePlayer = Cast<APrototype2Character>(OtherActor);
	if (!IsValid(SomePlayer))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Attempted to offer something!"));
	if (!IsValid(SomePlayer->HeldItem))
		return;
	
	if (auto Plant = Cast<APlant>(SomePlayer->HeldItem))
	{
		int32 StarValue = Plant->SeedData->BabyStarValue;
		StarValue = FMath::Clamp(StarValue, 1, 5);
		Attack(SomePlayer, StarValue);
		
		// Destroy the crop the player is holding
		SomePlayer->HeldItem->Destroy();
		SomePlayer->HeldItem = nullptr;
		if (IsValid(SomePlayer->PlayerHUDRef))
		{
			SomePlayer->PlayerHUDRef->ClearPickupUI();
			SomePlayer->PlayerHUDRef->SetHUDInteractText("");
		}
		SomePlayer->RefreshCurrentMaxSpeed();
	}
}

void ASkyAlterAttack::Attack(APrototype2Character* _PlayerToNotSmite, int32 _StarValueOfPlant)
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototype2Character::StaticClass(), AllPlayers);

	for(AActor* Player : AllPlayers)
	{
		// Check if player is the player sacrificing the plant
		if (Player == _PlayerToNotSmite)
		{
			continue;
		}
		
		APrototype2Character* CastedPlayer = Cast<APrototype2Character>(Player);
		if (CastedPlayer)
		{
			// Check if player is team mate
			if (CastedPlayer->PlayerStateRef->Details.Colour == _PlayerToNotSmite->PlayerStateRef->Details.Colour)
				continue;
			
			if (IsValid(SmiteWeaponData))
			{
				// Hit the player
				float KnockBack = KnockBackMultiplier * _StarValueOfPlant;
				CastedPlayer->InitiateSmite(KnockBack, SmiteWeaponData);
			}
			else
			{
				UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Smite Weapon Data");
			}
		}
		
	}
}

