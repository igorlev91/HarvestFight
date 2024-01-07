#include "EndGamePodium.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"

AEndGamePodium::AEndGamePodium()
{
	PrimaryActorTick.bCanEverTick = true;

	PodiumMeshes.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName("Podium Mesh 1")));
	RootComponent = PodiumMeshes[0];
	PodiumMeshes.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName("Podium Mesh 2")));
	PodiumMeshes[1]->SetupAttachment(PodiumMeshes[0]);
	
	P1LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P1 Lose Spawn Pos"));
	P1LosePosition->SetupAttachment(RootComponent);
	P2LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P2 Lose Spawn Pos"));
	P2LosePosition->SetupAttachment(RootComponent);
	P3LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P3 Lose Spawn Pos"));
	P3LosePosition->SetupAttachment(RootComponent);
	P4LosePosition = CreateDefaultSubobject<UArrowComponent>(FName("P4 Lose Spawn Pos"));
	P4LosePosition->SetupAttachment(RootComponent);
	
	P1WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P1 Win Spawn Pos"));
	P1WinPosition->SetupAttachment(RootComponent);
	P2WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P2 Win Spawn Pos"));
	P2WinPosition->SetupAttachment(RootComponent);
	P3WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P3 Win Spawn Pos"));
	P3WinPosition->SetupAttachment(RootComponent);
	P4WinPosition = CreateDefaultSubobject<UArrowComponent>(FName("P4 Win Spawn Pos"));
	P4WinPosition->SetupAttachment(RootComponent);
}

void AEndGamePodium::BeginPlay()
{
	Super::BeginPlay();

	if (EndGameCameraPrefab)
	{
		EndGameCamera = GetWorld()->SpawnActor<AEndGameCamera>(EndGameCameraPrefab ,
			FVector{755.20f,-309.04f,619.70f},
			FRotator{0.0f, 16.9f, 0.0f});
		EndGameCamera->SetReplicates(true);
		EndGameCamera->SetReplicatingMovement(true);
		EndGameCamera->AttachToComponent(PodiumMeshes[0], FAttachmentTransformRules::KeepRelativeTransform);
		EndGameCamera->SetActorRelativeLocation(FVector{-798.70f,-395.89f,509.32f});
		EndGameCamera->SetActorRelativeRotation(FRotator{-10.0f,18.41f,0.0f});
	}

	WinPositions.Add(P1WinPosition);
	WinPositions.Add(P2WinPosition);
	WinPositions.Add(P3WinPosition);
	WinPositions.Add(P4WinPosition);
	LossPositions.Add(P1LosePosition);
	LossPositions.Add(P2LosePosition);
	LossPositions.Add(P3LosePosition);
	LossPositions.Add(P4LosePosition);
}

void AEndGamePodium::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

void AEndGamePodium::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEndGamePodium, EndGameCamera);
}

AEndGameCamera* AEndGamePodium::GetEndGameCamera()
{
	return EndGameCamera;
}

UArrowComponent* AEndGamePodium::GetWinPosition(int32 _Placement)
{
	if (WinPositions.Num() > 0)
		return WinPositions[FMath::Clamp(0, WinPositions.Num() - 1, _Placement)];

	return nullptr;
}

UArrowComponent* AEndGamePodium::GetLossPosition(int32 _Placement)
{
	if (LossPositions.Num() > 0)
		return LossPositions[FMath::Clamp(0, LossPositions.Num() - 1, _Placement)];

	return nullptr;
}

