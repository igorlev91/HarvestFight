

#include "Prototype2/Gameplay/MovingPlatforms.h"

// Sets default values
AMovingPlatforms::AMovingPlatforms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

}

// Called when the game starts or when spawned
void AMovingPlatforms::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMovingPlatforms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeTrackerTimeToMove += DeltaTime;

	MovePlatform();
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
	if (PositionInMoveArray >= PositionArray.Num())
	{
		bMoveArrayBackwards = true;
		PositionInMoveArray -= 2;
		return;	
	}

	if (PositionInMoveArray == 0)
	{
		PositionInMoveArray++;
	}
	
	const FVector Position = FMath::Lerp<FVector>(PositionArray[PositionInMoveArray - 1], PositionArray[PositionInMoveArray], TimeTrackerTimeToMove / TimeToMove);
	SetActorLocation(Position);
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
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

	if (PositionInMoveArray == PositionArray.Num() - 1)
	{
		PositionInMoveArray--;
	}
	
	const FVector Position = FMath::Lerp<FVector>(PositionArray[PositionInMoveArray + 1], PositionArray[PositionInMoveArray], TimeTrackerTimeToMove / TimeToMove);
	SetActorLocation(Position);
	if  (TimeTrackerTimeToMove >= TimeToMove)
	{
		TimeTrackerTimeToMove = 0;
		PositionInMoveArray -= 1;
	}
}

