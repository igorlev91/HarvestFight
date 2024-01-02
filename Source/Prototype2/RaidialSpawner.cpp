// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidialSpawner.h"
#include "RadialPlot.h"
#include "Gamestates/Prototype2Gamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ARaidialSpawner::ARaidialSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaidialSpawner::BeginPlay()
{
	Super::BeginPlay();
	SetUp();
	
}

// Called every frame
void ARaidialSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARaidialSpawner::SetUp()
{
	if (auto* gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		playercount = gamestate->FinalConnectionCount;
	}
	
	FVector ReferenceLocation = FVector();
	if (initialSpawn)
	{
		ReferenceLocation = initialSpawn->GetActorLocation();
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

		// Spawn the object at the calculated position
		AActor* SpawnedObject = GetWorld()->SpawnActor<AActor>(plot, ObjectSpawnPosition, FRotator(0, Angle + 90, 0));
		
		if (SpawnedObject)
		{
			if (ARadialPlot* RadialPlot = Cast<ARadialPlot>(SpawnedObject))
			{
				RadialPlot->SetPlayerID(Index);
			}
		}
	}
	
}

