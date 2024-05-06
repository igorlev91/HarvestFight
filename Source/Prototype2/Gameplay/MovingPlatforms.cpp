

#include "Prototype2/Gameplay/MovingPlatforms.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/PickUpItem.h"

// Sets default values
AMovingPlatforms::AMovingPlatforms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	Mesh->SetupAttachment(RootComponent);
	bReplicates = true;
	Mesh->SetIsReplicated(true);
}

void AMovingPlatforms::BeginPlay()
{
	Super::BeginPlay();


}

void AMovingPlatforms::Tick(float DeltaSeconds)
{
	// DISABLED MOVING PLATFORMS //
	return;
	// DISABLED MOVING PLATFORMS //
	
	if (!bDoMove && !bDoRotate && !bDoSineWave)
		return;
	
	Super::Tick(DeltaSeconds);
	
	SetReplicateMovement(false);
	if (HasAuthority())
	{
		if (StartPosition == FVector{-1337.0f, -1337.0f, -1337.0f})
			StartPosition = GetActorLocation();
		
		// DO ALL MOVEMENT LOGIC INSIDE THIS FUNCTION
		DoMovementLogic(DeltaSeconds);

		// Store the current position and timestamp for prediction.
		PreviousPositions.Add(GetActorLocation());
		PreviousRotations.Add(GetActorRotation().Quaternion());
		Timestamps.Add(GetWorld()->GetTimeSeconds());

		// Remove old position data to keep the buffer size limited.
		if (PreviousPositions.Num() > MAX_POSITION_HISTORY)
		{
			PreviousPositions.RemoveAt(0);
			PreviousRotations.RemoveAt(0);
			Timestamps.RemoveAt(0);
		}
	}
	else
	{
		// Update the client's interpolated position when ReplicatedPlatformPosition changes.
		FVector NewLocation = GetActorLocation();
		FQuat NewRotation = GetActorRotation().Quaternion();

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
				NewLocation = (FMath::Lerp(PreviousPositions[Index1], PreviousPositions[Index2], Alpha));
				NewRotation = (FQuat::Slerp(PreviousRotations[Index1], PreviousRotations[Index2], Alpha));
			}
		}

		// Set the new actor location on the client.
		SetActorRelativeLocation(NewLocation);
		SetActorRelativeRotation(NewRotation);
	}
	
}

FVector AMovingPlatforms::PredictFuturePosition(float PredictionTime)
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

bool AMovingPlatforms::FindClosestTimestamps(float TargetTime, int32& Index1, int32& Index2)
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

void AMovingPlatforms::DoMovementLogic(float DeltaTime)
{
	// dont update both movement and sinewave
	if (bDoSineWave && bDoMove)
		return;

	if (InitialDelay > 0)
	{
		InitialDelay -= DeltaTime;
		return;
	}
	
	//Sine Wave
	SineWave(DeltaTime);
	MovePlatform(DeltaTime);
	//Rotate(DeltaTime);
}

void AMovingPlatforms::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMovingPlatforms, ReplicatedPlatformPosition);
	DOREPLIFETIME(AMovingPlatforms, ReplicatedPlatformRotation);
	DOREPLIFETIME(AMovingPlatforms, PreviousPositions);
	DOREPLIFETIME(AMovingPlatforms, StartPosition);
	DOREPLIFETIME(AMovingPlatforms, StartRotation);
	DOREPLIFETIME(AMovingPlatforms, PreviousRotations);
	DOREPLIFETIME(AMovingPlatforms, Timestamps);
}

void AMovingPlatforms::OnRep_PlatformPosition()
{
	// DISABLED MOVING PLATFORMS //
	return;
	
	// Update the client's interpolated position when ReplicatedPlatformPosition changes.
	FVector NewLocation = GetActorLocation();
	FQuat NewRotation = GetActorRotation().Quaternion();

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
			NewLocation = (FMath::Lerp(PreviousPositions[Index1], PreviousPositions[Index2], Alpha));
			NewRotation = (FQuat::Slerp(PreviousRotations[Index1], PreviousRotations[Index2], Alpha));
		}
	}

	// Set the new actor location on the client.
	SetActorRelativeLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(NewRotation);
}

void AMovingPlatforms::MovePlatform(float DeltaTime)
{
	if (!bDoMove)
		return;

	if (MovementInfoArray.Num() == 0)
		return;

	TimeStop -= DeltaTime;
	if (TimeStop >= 0)
		return;
	
	TimeTrackerTimeToMove += DeltaTime;

	if (HasAuthority())
	{
		TArray<AActor*> NearbyPickups{};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickUpItem::StaticClass(), NearbyPickups);

		for(AActor* NearbyActor : NearbyPickups)
		{
			if (FVector::Distance(GetActorLocation(), NearbyActor->GetActorLocation()) <= 300)
			{
				if (APickUpItem* NearbyPickup = Cast<APickUpItem>(NearbyActor))
				{
					NearbyPickup->ItemComponent->Mesh->SetSimulatePhysics(true);
				}
			}
		}
	}
	
	if (bDoLoop)
	{
		MovePlatformLoop();
		return;
	}
	
	if (!bMoveArrayBackwards)
	{
		MovePlatformForward();
	}
	else
	{
		MovePlatformBackwards();
	}
}

void AMovingPlatforms::MovePlatformForward()
{
	// make sure the array is at the correct point
	if (PositionInMoveArray >= MovementInfoArray.Num())
	{
		bMoveArrayBackwards = true;
		PositionInMoveArray -= 2;
		return;	
	}

	if (PositionInMoveArray == 0)
	{
		PositionInMoveArray++;
	}

	if (PositionInMoveArray >= MovementInfoArray.Num())
	{
		return;	
	}

	if (PositionInMoveArray == 0)
	{
		PositionInMoveArray++;
	}


	// moves the platform
	const FVector Position = FMath::Lerp<FVector>(MovementInfoArray[PositionInMoveArray - 1].Position, MovementInfoArray[PositionInMoveArray].Position, TimeTrackerTimeToMove / TimeToMove);
	
	ReplicatedPlatformPosition = Position;
	SetActorRelativeLocation(ReplicatedPlatformPosition, false, nullptr, ETeleportType::TeleportPhysics);

	/*if (bDoRotation)
	{
		FQuat NewRotation = FQuat::MakeFromEuler(FVector(StartRotation.X, StartRotation.Y, StartRotation.Z + Amplitude * FMath::Sin(SineTime)));
		SetActorRotation(NewRotation);
	}*/
	
	// update the needed variables for pausing
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
		// uses unique timers for each stop (not used ingame atm)
		if (!bUseBaseTimers)
		{
			TimeStop = MovementInfoArray[PositionInMoveArray].TimeToStopFor;
			TimeToMove = MovementInfoArray[PositionInMoveArray].TimeToMove;
		}
		else
		{
			TimeStop = BaseTimeStop;
		}
		TimeTrackerTimeToMove = 0;
		PositionInMoveArray += 1;
	}
}

void AMovingPlatforms::MovePlatformBackwards()
{
	// make sure the array is at the correct point
	if (PositionInMoveArray < 0)
	{
		bMoveArrayBackwards = false;
		PositionInMoveArray += 2;
		return;	
	}

	if (PositionInMoveArray == MovementInfoArray.Num() - 1)
	{
		PositionInMoveArray--;
	}

	if (PositionInMoveArray < 0)
	{
		return;	
	}

	if (PositionInMoveArray == MovementInfoArray.Num() - 1)
	{
		PositionInMoveArray--;
	}


	// moves the platform
	const FVector Position = FMath::Lerp<FVector>(MovementInfoArray[PositionInMoveArray + 1].Position, MovementInfoArray[PositionInMoveArray].Position, TimeTrackerTimeToMove / TimeToMove);

	ReplicatedPlatformPosition = Position;
	SetActorRelativeLocation(ReplicatedPlatformPosition, false, nullptr, ETeleportType::TeleportPhysics);

	/*if (bDoRotation)
	{
		FQuat NewRotation = FQuat::MakeFromEuler(FVector(StartRotation.X, StartRotation.Y, StartRotation.Z + Amplitude * FMath::Sin(SineTime)));
		SetActorRotation(NewRotation);
	}*/
	
	// update the needed variables for pausing
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
		// uses unique timers for each stop (not used ingame atm)
		if (!bUseBaseTimers)
		{
			TimeStop = MovementInfoArray[PositionInMoveArray].TimeToStopFor;
			TimeToMove = MovementInfoArray[PositionInMoveArray].TimeToMove;
		}
		else
		{
			TimeStop = BaseTimeStop;
		}
		TimeTrackerTimeToMove = 0;
		PositionInMoveArray -= 1;
	}
}

void AMovingPlatforms::MovePlatformLoop()
{
	if (PositionInMoveArray >= MovementInfoArray.Num())
	{
		PositionInMoveArray = 0;
		return;	
	}

	int32 PreviousPosition = PositionInMoveArray - 1;

	if (PreviousPosition < 0)
	{
		PreviousPosition = MovementInfoArray.Num() - 1;
	}

	if (PreviousPosition < 0 || PositionInMoveArray < 0)
		return;


	// moves the platform
	const FVector Position = FMath::Lerp<FVector>(MovementInfoArray[PreviousPosition].Position, MovementInfoArray[PositionInMoveArray].Position, TimeTrackerTimeToMove / TimeToMove);
	
	ReplicatedPlatformPosition = Position;
	SetActorRelativeLocation(ReplicatedPlatformPosition, false, nullptr, ETeleportType::TeleportPhysics);

	/*if (bDoRotation)
	{
		FQuat NewRotation = FQuat::MakeFromEuler(FVector(StartRotation.X, StartRotation.Y, StartRotation.Z + Amplitude * FMath::Sin(SineTime)));
		SetActorRotation(NewRotation);
	}*/
	
	// update the needed variables for pausing
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
		// uses unique timers for each stop (not used ingame atm)
		if (!bUseBaseTimers)
		{
			TimeStop = MovementInfoArray[PositionInMoveArray].TimeToStopFor;
			TimeToMove = MovementInfoArray[PositionInMoveArray].TimeToMove;
		}
		else
		{
			TimeStop = BaseTimeStop;
		}
		TimeTrackerTimeToMove = 0;
		PositionInMoveArray += 1;
	}
}

void AMovingPlatforms::Rotate(float DeltaTime)
{
	if (!bDoRotate)
		return;

	// doe if the platform isnt moving between positions, used for island rotations
	if (!bDoMove)
	{
		FRotator NewRotation = GetActorRotation() + FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f);
		ReplicatedPlatformRotation = NewRotation;
		SetActorRelativeRotation(ReplicatedPlatformRotation, false, nullptr, ETeleportType::TeleportPhysics);
		return;
	}

	// used if moving between different positions, used for screws, rotates in reverse when screws move downwards
	if (!bMoveArrayBackwards)
	{
		FRotator NewRotation = GetActorRotation() + FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f);
		ReplicatedPlatformRotation = NewRotation;
		SetActorRelativeRotation(ReplicatedPlatformRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		FRotator NewRotation = GetActorRotation() + FRotator(0.0f, -RotationSpeed * DeltaTime, 0.0f);
		ReplicatedPlatformRotation = NewRotation;
		SetActorRelativeRotation(ReplicatedPlatformRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AMovingPlatforms::SineWave(float DeltaTime)
{
	if (!bDoSineWave)
		return;
	
	if (SineTime >= 3.141592)
	{
		SineTime = -3.141592;
	}
	else
	{
		SineTime += DeltaTime * Frequency;
	}
	
	const int SineHeight = SineHeightBounce.Z;
	SineHeightBounce = StartPosition;
	SineHeightBounce.Z = SineHeight;
	
	FVector NewLocation = SineHeightBounce + FVector(0.0f, 0.0f, Amplitude * FMath::Sin(SineTime));
	
	//
	// ALWAYS SET ReplicatedPlatformPosition
	ReplicatedPlatformPosition = NewLocation;
	SetActorRelativeLocation(ReplicatedPlatformPosition, false, nullptr, ETeleportType::TeleportPhysics);

	/*if (bDoRotation)
	{
		FQuat NewRotation = FQuat::MakeFromEuler(FVector(StartRotation.X, StartRotation.Y, StartRotation.Z + Amplitude * FMath::Sin(SineTime)));
		SetActorRotation(NewRotation);
	}*/
}

//void AMovingPlatforms::ServerMovePlatform_Implementation(FVector NewLocation, FRotator NewRotation)
//{
//	SetActorLocation(NewLocation);
//	SetActorRotation(NewRotation);
//}

