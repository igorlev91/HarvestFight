

#include "ClockworkPlatform.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AClockworkPlatform::AClockworkPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Platform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	Platform->SetupAttachment(RootComponent);
	Platform->SetWorldScale3D(FVector::One() * 0.75f);
	
	Pole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pole"));
	Pole->SetupAttachment(RootComponent);

	// Debug Endpoint Sphere
	EndPoint_DEBUG = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DEBUG: End Point"));
	EndPoint_DEBUG->SetupAttachment(Platform);
	EndPoint_DEBUG->SetRelativeLocation(FVector::UpVector * 200.0f);
	EndPoint_DEBUG->SetCollisionProfileName("NoCollision");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMesh.Object != NULL)
	{
		EndPoint_DEBUG->SetStaticMesh(SphereMesh.Object);
		EndPoint_DEBUG->SetRelativeScale3D(FVector::One() * 0.5f);
	}
	EndPoint_DEBUG->SetRelativeLocation(FVector::UpVector * 853.33f);

	LerpTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> LinearCurve(TEXT("/Game/Curves/LinearCurve"));
	if (LinearCurve.Object != NULL)
	{
		FOnTimelineFloat TimelineCallback{};
		TimelineCallback.BindDynamic(this, &AClockworkPlatform::TickTimeline);
		LerpTimeline->AddInterpFloat(LinearCurve.Object, TimelineCallback);
		FOnTimelineEvent FinishedCallback{};
		FinishedCallback.BindDynamic(this, &AClockworkPlatform::OnTimelineEnd);
		LerpTimeline->SetTimelineFinishedFunc(FinishedCallback);
	}

	HaltTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Halt Timeline"));
	if (LinearCurve.Object != NULL)
	{
		FOnTimelineEvent FinishedCallback{};
		FinishedCallback.BindDynamic(this, &AClockworkPlatform::OnHaltEnd);
		HaltTimeline->SetTimelineFinishedFunc(FinishedCallback);
	}
}

void AClockworkPlatform::BeginPlay()
{
	Super::BeginPlay();
	EndPoint_DEBUG->SetVisibility(false);

	
	if (HasAuthority())
	{
		EndPoint_DEBUG->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		StartPosition = GetActorLocation();
		SetReplicateMovement(true);

		LerpTimeline->SetLooping(false);
		LerpTimeline->SetTimelineLength(5.0f);
		HaltTimeline->SetLooping(false);
		HaltTimeline->SetTimelineLength(5.0f);
	}
}

void AClockworkPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClockworkPlatform::TickTimeline(float _Progress)
{
	SetActorLocation(FMath::Lerp(StartPosition, EndPoint_DEBUG->GetComponentLocation(), _Progress));
}

void AClockworkPlatform::OnTimelineEnd()
{
	OnReturnJourney = !OnReturnJourney;
	HaltTimeline->PlayFromStart();
}

void AClockworkPlatform::OnHaltEnd()
{
	if (OnReturnJourney)
	{
		LerpTimeline->ReverseFromEnd();
	}
	else
	{
		LerpTimeline->PlayFromStart();
	}
}
