

#include "Prototype2/Gameplay/MovingPlatforms.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMovingPlatforms::AMovingPlatforms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	if(GetOwner())
	{
		GetOwner()->SetRootComponent(Mesh);
	}

	Mesh->SetRenderCustomDepth(false);
	Mesh->CustomDepthStencilValue = 1;

	bReplicates = true;
    AActor::SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AMovingPlatforms::BeginPlay()
{
	Super::BeginPlay();

	SineHeightBounce = GetActorLocation();

	SineTime = FMath::RandRange(SineStartTimeMin * 100, SineStartTimeMax * 100);
	SineTime /= 100;
}

// Called every frame
void AMovingPlatforms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDoSineWave)
		SineWave(DeltaTime);

	if (bDoMove)
	{
		TimeStop -= DeltaTime;

		if (TimeStop > 0)
			return;

		TimeTrackerTimeToMove += DeltaTime;

		MovePlatform();
	}

	if (bDoRotate)
	{
		Rotate(DeltaTime);
	}
}

void AMovingPlatforms::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMovingPlatforms, Mesh);
}

void AMovingPlatforms::MovePlatform()
{
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
	
	const FVector Position = FMath::Lerp<FVector>(MovementInfoArray[PositionInMoveArray - 1].Position, MovementInfoArray[PositionInMoveArray].Position, TimeTrackerTimeToMove / TimeToMove);
	SetActorLocation(Position);
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
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
	
	const FVector Position = FMath::Lerp<FVector>(MovementInfoArray[PositionInMoveArray + 1].Position, MovementInfoArray[PositionInMoveArray].Position, TimeTrackerTimeToMove / TimeToMove);
	SetActorLocation(Position);
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
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

void AMovingPlatforms::Rotate(float DeltaTime)
{
	FRotator NewRotation = GetActorRotation() + FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f);
	SetActorRotation(NewRotation);
}

void AMovingPlatforms::SineWave(float DeltaTime)
{
	SineTime += DeltaTime * Frequency;

	const int SineHeight = SineHeightBounce.Z;
	SineHeightBounce = GetActorLocation();
	SineHeightBounce.Z = SineHeight;

	const FVector NewLocation = SineHeightBounce + FVector(0.0f, 0.0f, Amplitude * FMath::Sin(SineTime));
	SetActorLocation(NewLocation);
}

