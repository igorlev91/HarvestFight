


#include "Prototype2/Spawning/FallOffRespawn.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Pickups/PickUpItem.h"

// Sets default values
AFallOffRespawn::AFallOffRespawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AFallOffRespawn::RemovePhysicsObjectsBelowHeight()
{
	TArray<AActor*> AllPickUps{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickUpItem::StaticClass(), AllPickUps);
	for(AActor* PickUp : AllPickUps)
	{
		if (PickUp->GetActorLocation().Z < RespawnHeight)
		{
			PickUp->Destroy();
		}
	}
}

void AFallOffRespawn::RespawnPlayersBelowHeight()
{
	TArray<AActor*> AllPlayers{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototype2Character::StaticClass(), AllPlayers);
	for(AActor* Player : AllPlayers)
	{
		if (Player->GetActorLocation().Z < RespawnHeight)
		{
			if (auto* Character = Cast<APrototype2Character>(Player))
			{
				if (Character->HeldItem)
				{
					Character->DropItem();
				}
			}
			
			Player->SetActorLocation(RespawnLocation);
		}
	}
}

void AFallOffRespawn::CheckTimer(float DeltaTime)
{
	TimeCounter -= DeltaTime;

	if (TimeCounter < 0)
		return;

	RemovePhysicsObjectsBelowHeight();
	RespawnPlayersBelowHeight();
	TimeCounter = TimeBetweenChecks;
}

// Called when the game starts or when spawned
void AFallOffRespawn::BeginPlay()
{
	Super::BeginPlay();
	TimeCounter = TimeBetweenChecks;
	RespawnHeight = GetActorLocation().Z;
}

// Called every frame
void AFallOffRespawn::Tick(float DeltaTime)
{
	CheckTimer(DeltaTime);
}

