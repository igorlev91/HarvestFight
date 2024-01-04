// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidialSpawner.h"

#include "GrowSpot.h"
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
			return;
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

		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				if (auto newPlot = GetWorld()->SpawnActor<AGrowSpot>(PlotPrefab, RootComponent->GetComponentTransform()))
				{
					newPlot->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					int distance = 180;
					newPlot->SetActorRelativeLocation(ObjectSpawnPosition + FVector{((float)i - 1.5f) * distance, ((float)j - 1.5f) * distance, 100.0f});
					newPlot->Player_ID = Index;

					FHitResult HitResult;
					FVector Start = newPlot->GetActorLocation();
					FVector End = Start + FVector(0, 0, -1000);
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(newPlot);

					bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
					if (bHit)
					{
						newPlot->SetActorLocation(FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z));
						//newPlot->SetActorRotation(FRotator(HitResult.ImpactNormal.Rotation().Pitch + 90, 0, HitResult.ImpactNormal.Rotation().Roll + 180));
					}
				}
			}
		}
	}
}

