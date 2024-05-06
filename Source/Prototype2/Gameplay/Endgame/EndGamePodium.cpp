#include "EndGamePodium.h"

#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Sound/SoundCue.h"

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

	WinConfetteComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Win Confette"));
	WinConfetteComponent->SetupAttachment(RootComponent);
	WinConfetteComponent->bAutoActivate = false;
	WinConfetteComponent->SetRelativeLocation({0.0f, 300.0f, 300.0f});

	SecondWinConfetteComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Second Win Confette"));
	SecondWinConfetteComponent->SetupAttachment(RootComponent);
	SecondWinConfetteComponent->bAutoActivate = false;
	SecondWinConfetteComponent->SetRelativeLocation({0.0f, -300.0f, 300.0f});
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> WinConfetteVFX(TEXT("/Game/VFX/AlphaVFX/NiagaraSystems/NS_WinConfetti"));
	if (WinConfetteVFX.Object != NULL)
	{
		WinConfetteComponent->SetAsset(WinConfetteVFX.Object);
		SecondWinConfetteComponent->SetAsset(WinConfetteVFX.Object);
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
			EndGameCamera->SetActorRelativeLocation(CameraPosition);
			EndGameCamera->SetActorRelativeRotation(CameraRotation);
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

void AEndGamePodium::PlayConfetteVFX()
{
	if (HasAuthority())
	{
		Multi_PlayConfetteVFX();
	}
	else
	{
		Server_PlayConfetteVFX();
	}
}

void AEndGamePodium::Multi_PlayConfetteVFX_Implementation()
{
	WinConfetteComponent->Activate(true);
	SecondWinConfetteComponent->Activate(true);
	
	//if (ConfettiCue)
	//	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ConfettiCue, GetActorLocation());
}

void AEndGamePodium::Server_PlayConfetteVFX_Implementation()
{
	Multi_PlayConfetteVFX();
}

