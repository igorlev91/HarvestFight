


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

	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		PoofSystem = PoofVFX.Class;
	}
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
	//if (!OtherActor->HasNetOwner())
	//	return;
	
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
		UKismetSystemLibrary::PrintString(GetWorld(), "Sacrificed something");
		if (HasAuthority())
		{
			Multi_SacrificePoof();
			OnSacrificed();
		}
		else
		{
			if (PoofSystem)
			{
				FVector PoofSpot = GetActorLocation();
				PoofSpot.Z = PoofSpot.Z + 130.0f;
				auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, PoofSpot, FRotator{});
				SpawnedVFX->SetActorScale3D(FVector::One() * 3);
				SpawnedVFX->SetLifeSpan(5.0f);
			}
			OnSacrificed();
		}
		// audio
		SomePlayer->PlaySoundAtLocation(GetActorLocation(), SomePlayer->SacrificeCue, SomePlayer->AltarAttenuationSettings);
		
		int32 StarValue = Plant->ServerData.SeedData->BabyStarValue;
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
			if (!DEBUG_AttackTeamMates)
			{
				// Check if player is team mate
				if (CastedPlayer->PlayerStateRef->Details.Colour == _PlayerToNotSmite->PlayerStateRef->Details.Colour)
					continue;
			}
			
			if (IsValid(SmiteWeaponData))
			{
				// Hit the player
				float KnockBack = KnockBackMultiplier * _StarValueOfPlant;
				CastedPlayer->InitiateSmite(KnockBack, SmiteWeaponData);
				
				UKismetSystemLibrary::PrintString(GetWorld(), "InitiatedSmite()");
			}
			else
			{
				UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Smite Weapon Data");
			}
		}
		
	}
}


void ASkyAlterAttack::Server_SacrificePoof_Implementation()
{
	Multi_SacrificePoof();
}

void ASkyAlterAttack::Multi_SacrificePoof_Implementation()
{
	if (PoofSystem)
	{
		FVector PoofSpot = GetActorLocation();
		PoofSpot.Z = PoofSpot.Z + 130.0f;
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, PoofSpot, FRotator{});
		SpawnedVFX->SetActorScale3D(FVector::One() * 3);
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}