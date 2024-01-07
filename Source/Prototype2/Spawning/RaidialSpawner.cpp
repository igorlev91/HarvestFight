

#include "RaidialSpawner.h"

#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Gameplay/RadialPlot.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ARaidialSpawner::ARaidialSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	InitialSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Initial Spawn"));
	InitialSpawn->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARaidialSpawner::BeginPlay()
{
	Super::BeginPlay();

	
	if (HasAuthority())
	{
		if (PlotPrefab)
			SetUp();
	}
}

void ARaidialSpawner::SetUp()
{
	if (bShouldChangePlayerCount)
	{
		if (auto GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			PlayerCount = GameState->GetFinalConnectionCount();
		}
		if (PlayerCount <= 0)
		{
			PlayerCount = 1;
		}
	}
	
	FVector ReferenceLocation = FVector();
	if (InitialSpawn)
	{
		ReferenceLocation = InitialSpawn->GetComponentLocation();
	}
	FVector OwningLocation = GetActorLocation();
	//FVector OwningLocation = FVector();
	float SpawnRadius = FVector::Dist(ReferenceLocation, OwningLocation);

	float AngleStep = 360.0f / PlayerCount; // Calculate the angle between each object

	for (int32 Index = 0; Index < PlayerCount; Index++)
	{
		// Calculate the angle for the current object
		float Angle = Index * AngleStep;

		// Convert angle to radians
		float Radians = FMath::DegreesToRadians(Angle);

		// Calculate the spawn position offset based on angle and radius
		float OffsetX = SpawnRadius * FMath::Cos(Radians);
		float OffsetY = SpawnRadius * FMath::Sin(Radians);
		
		// Calculate the final spawn position
		FVector ObjectSpawnPosition = OwningLocation + FVector(150, 150, 0) + FVector(OffsetX, OffsetY, 0.0f);

		// Spawn and align center plot

		ARadialPlot* Plot = GetWorld()->SpawnActor<ARadialPlot>(PlotPrefab);
		Plot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		Plot->SetActorRelativeLocation(ObjectSpawnPosition);
		Plot->SetPlayerID(Index);

		FVector SourceLocation = Plot->GetActorLocation();
		FVector Direction = OwningLocation - SourceLocation;
		Direction.Z = 0.0f;
		FRotator DesiredRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		float RotationAngle = DesiredRotation.Yaw;
		Plot->SetActorRotation({Plot->GetActorRotation().Pitch, RotationAngle, Plot->GetActorRotation().Roll});

		Plots.Add(Index, Plot);
	}
}

void ARaidialSpawner::SetupDelayed()
{
	for (const TPair<int32, ARadialPlot*>& Pair : Plots)
	{
		Pair.Value->SetPlayerID(Pair.Key);		
	}
}

