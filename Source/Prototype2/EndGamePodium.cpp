// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGamePodium.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/EndGameCamera.h"

// Sets default values
AEndGamePodium::AEndGamePodium()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PodiumMeshes.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName("Podium Mesh 1")));
	RootComponent = PodiumMeshes[0];
	PodiumMeshes.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName("Podium Mesh 2")));
	PodiumMeshes[1]->SetupAttachment(PodiumMeshes[0]);
	
	// Lose positions for each player
	P1LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P1 Lose Spawn Pos"));
	P1LosePosition->SetupAttachment(RootComponent);
	P2LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P2 Lose Spawn Pos"));
	P2LosePosition->SetupAttachment(RootComponent);
	P3LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P3 Lose Spawn Pos"));
	P3LosePosition->SetupAttachment(RootComponent);
	P4LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P4 Lose Spawn Pos"));
	P4LosePosition->SetupAttachment(RootComponent);

	// Win positions for each player
	P1WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P1 Win Spawn Pos"));
	P1WinPosition->SetupAttachment(RootComponent);
	P2WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P2 Win Spawn Pos"));
	P2WinPosition->SetupAttachment(RootComponent);
	P3WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P3 Win Spawn Pos"));
	P3WinPosition->SetupAttachment(RootComponent);
	P4WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P4 Win Spawn Pos"));
	P4WinPosition->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEndGamePodium::BeginPlay()
{
	Super::BeginPlay();

	if (EndGameCameraPrefab)
	{
		EndGameCamera = GetWorld()->SpawnActor<AEndGameCamera>(EndGameCameraPrefab ,FVector{755.20f,-309.04f,619.70f}, FRotator{0.0f, 16.9f, 0.0f});
		//EndGameCamera->AttachToComponent(ThirdPlaceMeshes[0], FAttachmentTransformRules::KeepRelativeTransform);
		EndGameCamera->SetReplicates(true);
		EndGameCamera->SetReplicatingMovement(true);
		EndGameCamera->AttachToComponent(PodiumMeshes[0], FAttachmentTransformRules::KeepRelativeTransform);
		EndGameCamera->SetActorRelativeLocation(FVector{-798.702375,-395.890137,509.317685});
		EndGameCamera->SetActorRelativeRotation(FRotator{-10.000000,18.409999,-0.000000});
	}
}

// Called every frame
void AEndGamePodium::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEndGamePodium::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEndGamePodium, EndGameCamera);
}

