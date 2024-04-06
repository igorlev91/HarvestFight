

#include "AspearagusProjectile.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/Gameplay/SkyAlter.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"


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
	AspearagusMesh->SetIsReplicated(true);
	AspearagusMesh->SetVisibility(true);
	AspearagusMesh->SetSimulatePhysics(false);
	AspearagusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AspearagusMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

FVector AAspearagusProjectile::PredictFuturePosition(float PredictionTime)
{
	// Calculate the predicted position based on the current position, velocity, and time.
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float TargetTime = CurrentTime + PredictionTime;

	// Find the two closest timestamps for interpolation.
	int32 Index1, Index2;
	if (FindClosestTimestamps(TargetTime, Index1, Index2))
	{
		float Alpha = (TargetTime - Timestamps[Index1]) / (Timestamps[Index2] - Timestamps[Index1]);
		FVector PredictedTransform = FMath::Lerp(PreviousPositions[Index1], PreviousPositions[Index2], Alpha);
		return PredictedTransform;
	}

	// If prediction is not possible, return the current position.
	return GetActorLocation();
}

bool AAspearagusProjectile::FindClosestTimestamps(float TargetTime, int32& Index1, int32& Index2)
{
	// Ensure there are at least two timestamps for comparison.
	if (Timestamps.Num() < 2)
		return false;

	int32 LowIndex = 0;
	int32 HighIndex = Timestamps.Num() - 1;

	while (LowIndex < HighIndex)
	{
		int32 MidIndex = (LowIndex + HighIndex) / 2;

		if (Timestamps[MidIndex] == TargetTime)
		{
			// If we found an exact match, set both indices to this index.
			Index1 = MidIndex;
			Index2 = MidIndex;
			return true;
		}
		else if (Timestamps[MidIndex] < TargetTime)
		{
			LowIndex = MidIndex + 1;
		}
		else
		{
			HighIndex = MidIndex;
		}
	}

	// At this point, LowIndex is the closest timestamp before or equal to TargetTime,
	// and HighIndex is the closest timestamp after TargetTime.
	Index1 = LowIndex;
	Index2 = HighIndex;

	return true;
}

void AAspearagusProjectile::DoMovementLogic(float DeltaTime)
{
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime);
}

void AAspearagusProjectile::OnRep_PlatformPosition()
{
	// Update the client's interpolated position when ReplicatedPlatformPosition changes.
	FVector NewLocation = GetActorLocation();

	// Get the current client time.
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Interpolate between ReplicatedPlatformPosition and the predicted position.
	if (PreviousPositions.Num() >= 2)
	{
		int32 Index1, Index2;
		if (FindClosestTimestamps(CurrentTime, Index1, Index2))
		{
			float Timestamp1 = Timestamps[Index1];
			float Timestamp2 = Timestamps[Index2];
			float Alpha = 1.0f;

			if (Timestamp2 != Timestamp1)
			{
				// Calculate the interpolation alpha based on time.
				Alpha = (CurrentTime - Timestamp1) / (Timestamp2 - Timestamp1);
			}
			
			// Interpolate between the two positions.
			NewLocation = (FMath::Lerp(GetActorLocation(), PreviousPositions[Index2], Alpha));
		}
	}

	// Set the new actor location on the client.
	SetActorRelativeLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

// Called every frame
void AAspearagusProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetReplicateMovement(false);
	if (HasAuthority())
	{
		if (StartPosition == FVector{-1337.0f, -1337.0f, -1337.0f})
			StartPosition = GetActorLocation();
		
		// DO ALL MOVEMENT LOGIC INSIDE THIS FUNCTION
		DoMovementLogic(DeltaTime);

		// Store the current position and timestamp for prediction.
		PreviousPositions.Add(GetActorLocation());
		Timestamps.Add(GetWorld()->GetTimeSeconds());

		// Remove old position data to keep the buffer size limited.
		if (PreviousPositions.Num() > 2)
		{
			PreviousPositions.RemoveAt(0);
			Timestamps.RemoveAt(0);
		}

		if (DeathTimer > 0)
		{
			DeathTimer -= DeltaTime;

			if (!bHitSomething)
			{
				CheckForHitObstacle();
				CheckForHitPlayers();
			}

			if (DeathTimer <= 0 && HasAuthority())
			{
				Destroy();
			}
		}
	}
	else
	{
		// Update the client's interpolated position when ReplicatedPlatformPosition changes.
		FVector NewLocation = GetActorLocation();

		// Get the current client time.
		float CurrentTime = GetWorld()->GetTimeSeconds();

		// Interpolate between ReplicatedPlatformPosition and the predicted position.
		if (PreviousPositions.Num() >= 2)
		{
			int32 Index1, Index2;
			if (FindClosestTimestamps(CurrentTime, Index1, Index2))
			{
				float Timestamp1 = Timestamps[Index1];
				float Timestamp2 = Timestamps[Index2];
				float Alpha = 1.0f;

				if (Timestamp2 != Timestamp1)
				{
					// Calculate the interpolation alpha based on time.
					Alpha = (CurrentTime - Timestamp1) / (Timestamp2 - Timestamp1);
				}

				// Interpolate between the two positions.
				NewLocation = (FMath::Lerp(GetActorLocation(), PreviousPositions[Index2], Alpha));
			}
		}

		// Set the new actor location on the client.
		SetActorRelativeLocation(NewLocation);
	}
	
	
}

void AAspearagusProjectile::CheckForHitPlayers()
{
	if (!OwningPlayer)
		return;

	if (bHitSomething)
		return;
	
	// create a collision sphere
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(50.0f);

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
	if (GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldDynamic , CollisionSphere))
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
			else if (Cast<AGrowSpot>(HitResult.GetActor()))
			{
				bHitSomething = false;
			}
			else if (Cast<APickUpItem>(HitResult.GetActor()))
			{
				bHitSomething = false;
			}
			else
			{
				bHitSomething = true;
			}
		}
	}
	return bHitSomething;
}

void AAspearagusProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAspearagusProjectile, AttackSphereRadius);
	DOREPLIFETIME(AAspearagusProjectile, OwningPlayer);
	DOREPLIFETIME(AAspearagusProjectile, AspearagusMesh);
	DOREPLIFETIME(AAspearagusProjectile, ChargeAmount);
	DOREPLIFETIME(AAspearagusProjectile, Speed);
}

void AAspearagusProjectile::Server_InitializeProjectile_Implementation(APrototype2Character* _Player,
	UStaticMesh* _Mesh, float _Speed, float _LifeTime, float _AttackSphereRadius, float _AttackChargeAmount)
{
	OwningPlayer = _Player;
	AspearagusMesh->SetStaticMesh(_Mesh);
	Speed = _Speed;
	DeathTimer = _LifeTime;
	AttackSphereRadius = _AttackSphereRadius * _Player->CurrentWeaponSeedData->WeaponData->ScaleOfAOELargerThanIndicator;
	ChargeAmount = _AttackChargeAmount;
}

