#include "EndGamePodium.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"

AEndGamePodium::AEndGamePodium()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Base Root Component"));
	RootComponent = BaseRootComponent;

	bReplicates = true;
	
	for(int32 i = 0; i < 3; i++)
	{
		FName EngineName{"Podium Mesh (" + FString::FromInt(i) + ")"};
		PodiumMeshes.EmplaceAt(i, CreateDefaultSubobject<UStaticMeshComponent>(EngineName));
		PodiumMeshes[i]->SetupAttachment(RootComponent);
	}
	
	for(int32 i = 0; i < 6; i++)
	{
		FName EngineName{"Player " + FString::FromInt(i) + " Win Position"};
		WinPositions.EmplaceAt(i, CreateDefaultSubobject<UArrowComponent>(EngineName));
		WinPositions[i]->SetupAttachment(RootComponent);
	}
	for(int32 i = 0; i < 6; i++)
	{
		FName EngineName{"Player " + FString::FromInt(i) + " Loss Position"};
		LossPositions.EmplaceAt(i, CreateDefaultSubobject<UArrowComponent>(EngineName));
		LossPositions[i]->SetupAttachment(RootComponent);
	}
}

void AEndGamePodium::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);

	if (HasAuthority())
	{
		if (EndGameCameraPrefab && !EndGameCamera)
		{
			EndGameCamera = GetWorld()->SpawnActor<ACameraActor>(EndGameCameraPrefab);
			EndGameCamera->AttachToComponent(BaseRootComponent, FAttachmentTransformRules::KeepWorldTransform);
			EndGameCamera->SetActorRelativeLocation({560.000000,90.000000,170.000000});
			EndGameCamera->SetActorRelativeRotation({5.625000,185.625000,-0.000000});
			EndGameCamera->SetReplicates(true);
			EndGameCamera->SetReplicateMovement(true);
		}
	}
}

void AEndGamePodium::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

void AEndGamePodium::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEndGamePodium, BaseRootComponent);
	DOREPLIFETIME(AEndGamePodium, PodiumMeshes);
	DOREPLIFETIME(AEndGamePodium, WinPositions);
	DOREPLIFETIME(AEndGamePodium, LossPositions);
	DOREPLIFETIME(AEndGamePodium, EndGameCamera);
}

ACameraActor* AEndGamePodium::GetEndGameCamera()
{
	//if (EndGameCameraPrefab && !EndGameCamera)
	//{
	//	EndGameCamera = GetWorld()->SpawnActor<ACameraActor>(EndGameCameraPrefab);
	//	EndGameCamera->AttachToComponent(BaseRootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	//	EndGameCamera->SetActorRelativeLocation({543.212158,64.671567,190.000000});
	//	EndGameCamera->SetActorRelativeRotation({2.812500,-165.937499,0.000000});
	//}
	
	return EndGameCamera;
}

UArrowComponent* AEndGamePodium::GetWinPosition(int32 _Placement)
{
	if (WinPositions.Num() > 0)
		return WinPositions[FMath::Clamp(_Placement, 0, WinPositions.Num() - 1)];

	return nullptr;
}

UArrowComponent* AEndGamePodium::GetLossPosition(int32 _Placement)
{
	if (LossPositions.Num() > 0)
		return LossPositions[FMath::Clamp(_Placement, 0, WinPositions.Num() - 1)];

	return nullptr;
}

