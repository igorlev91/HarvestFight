

#include "ClockworkPlatform.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AClockworkPlatform::AClockworkPlatform()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat> LinearCurve(TEXT("/Game/Curves/LinearCurve"));
	if (LinearCurve.Object != nullptr)
	{
		MovementCurve = LinearCurve.Object;
	}
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;


	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Mesh"));
	SetRootComponent(PlatformMesh);

	StartPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Start Position (ROOT)"));
	StartPosition->SetupAttachment(RootComponent);

	EndPosition = CreateDefaultSubobject<USceneComponent>(TEXT("End Position"));
	EndPosition->SetupAttachment(RootComponent);
}

void AClockworkPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartPosition->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	EndPosition->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	SetReplicateMovement(false);

	if (HasAuthority())
	{
		TArray<AActor*> FoundPlatforms;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(), FoundPlatforms);

		for(int i = 0; i < FoundPlatforms.Num(); i++)
		{
			if (FoundPlatforms[i] == this)
			{
				PlatformTimeOffset = i * 2;
				break;
			}
		}
	}
}

void AClockworkPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority())
		ServerTime = GetWorld()->GetTimeSeconds();
	
	DoMovement();
}

void AClockworkPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AClockworkPlatform, ServerTime);
	DOREPLIFETIME(AClockworkPlatform, PlatformTimeOffset);
}

void AClockworkPlatform::DoMovement()
{
	if (IsValid(LocalPlayerState))
	{
		ObservingPlayerPing = LocalPlayerState->GetPingInMilliseconds();
		UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(ObservingPlayerPing));
	}
	
	float MoveAlpha = ((GetWorld()->GetTimeSeconds() * MoveSpeed) + PlatformTimeOffset + ClientTimeOffset + (ObservingPlayerPing/1000.0f)) * 0.05f;
	auto PredictedPos = FMath::Lerp(StartPosition->GetComponentLocation(), EndPosition->GetComponentLocation(), MovementCurve->FloatCurve.Eval(TriangleWave(MoveAlpha)));
	PlatformMesh->SetWorldLocation(FMath::Lerp(PlatformMesh->GetComponentLocation(), PredictedPos, GetWorld()->DeltaRealTimeSeconds * 3.0f));
}

float AClockworkPlatform::TriangleWave(float _X)
{
	float Result = sin((_X * PI * 2) + (PI / 2.0f));
	Result = acos(Result);
	Result = Result / (PI / 2.0f);
	Result = Result / 2.0f;
	return Result;
}

void AClockworkPlatform::OnRep_ServerTime()
{
	LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (IsValid(LocalPlayer))
	{
		LocalPlayerController = LocalPlayer->GetPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			LocalPlayerState = LocalPlayerController->GetPlayerState<APlayerState>();
			if (IsValid(LocalPlayerState))
			{
				ObservingPlayerPing = LocalPlayerState->GetPingInMilliseconds();
				UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(ObservingPlayerPing));
			}
		}
	}

	if (ClientTimeOffset == 0)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		ClientTimeOffset = ServerTime - CurrentTime;
		ClientTimeOffset += ObservingPlayerPing / 1000.0f;
	}
}
