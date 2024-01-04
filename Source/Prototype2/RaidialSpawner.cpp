// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidialSpawner.h"

#include "GrowSpot.h"
#include "Prototype2GameMode.h"
#include "RadialPlot.h"
#include "Gamestates/Prototype2Gamestate.h"
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
	if (changePlayerCount)
	{
		if (auto gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			playercount = gamestate->FinalConnectionCount;
		}
		if (playercount <= 0)
		{
			playercount = 1;
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

	float AngleStep = 360.0f / playercount; // Calculate the angle between each object

	for (int32 Index = 0; Index < playercount; Index++)
	{
		// Calculate the angle for the current object
		float Angle = Index * AngleStep;

		// Convert angle to radians
		float Radians = FMath::DegreesToRadians(Angle);

		// Calculate the spawn position offset based on angle and radius
		float OffsetX = SpawnRadius * FMath::Cos(Radians);
		float OffsetY = SpawnRadius * FMath::Sin(Radians);
		
		// Calculate the final spawn position
		FVector SpawnLocation = OwningLocation;
		FVector ObjectSpawnPosition = SpawnLocation + FVector(OffsetX, OffsetY, 0.0f);

		// Spawn and align center plot

		auto plot = GetWorld()->SpawnActor<ARadialPlot>(PlotPrefab);
		plot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		plot->SetActorRelativeLocation(ObjectSpawnPosition);
		plot->Player_ID = Index;

		FVector sourceLocation = plot->GetActorLocation();
		FVector targetLocation = RootComponent->GetComponentLocation();
		FVector direction = targetLocation - sourceLocation;
		direction.Z = 0.0f;
		FRotator desiredRotation = FRotationMatrix::MakeFromX(direction).Rotator();
		float rotationAngle = desiredRotation.Yaw;
		plot->SetActorRelativeRotation({plot->GetActorRotation().Pitch, rotationAngle, plot->GetActorRotation().Roll});
		plot->SetPlayerID(Index);
		
		/*auto centerPlot = GetWorld()->SpawnActor<AGrowSpot>(PlotPrefab);
		int distance = 400;
		{
			centerPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			centerPlot->SetActorRelativeLocation(ObjectSpawnPosition);
			centerPlot->Player_ID = Index;

			FVector sourceLocation = centerPlot->GetActorLocation();
			FVector targetLocation = RootComponent->GetComponentLocation();
			FVector direction = targetLocation - sourceLocation;
			direction.Z = 0.0f;
			FRotator desiredRotation = FRotationMatrix::MakeFromX(direction).Rotator();
			float rotationAngle = desiredRotation.Yaw;
			centerPlot->SetActorRelativeRotation({centerPlot->GetActorRotation().Pitch, rotationAngle, centerPlot->GetActorRotation().Roll});
		}
		
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				if (j == 1 && i == 1)
				{
				}
				else
				{
					if (auto newPlot = GetWorld()->SpawnActor<AGrowSpot>(PlotPrefab))
					{ 
						newPlot->AttachToActor(centerPlot, FAttachmentTransformRules::SnapToTargetIncludingScale);
						newPlot->SetActorRelativeLocation(FVector{(((float)i - 1.5f) * distance) + (distance / (2)), (((float)j - 1.5f) * distance)  + (distance / (2)), 0});
						newPlot->Player_ID = Index;
					}
				}
			}
		}*/
	}
}

