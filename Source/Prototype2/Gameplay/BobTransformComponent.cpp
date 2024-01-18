
#include "BobTransformComponent.h"

#include "Net/UnrealNetwork.h"

void UBobTransformComponent::SetEnabled(bool _bEnabled)
{
	bIsBobbing = _bEnabled;
}

UBobTransformComponent::UBobTransformComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBobTransformComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	if (!Owner) return;
	
	if (auto Mesh = Owner->GetComponentByClass(UStaticMeshComponent::StaticClass()))
		OwnerMesh = Cast<UStaticMeshComponent>(Mesh);
	if (!OwnerMesh) return;
}

void UBobTransformComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBobTransformComponent, StartTime);
}

void UBobTransformComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsBobbing)
		BobParentOnTick(DeltaTime);
}

void UBobTransformComponent::BobParentOnTick(float _DeltaTime)
{
	if (!OwnerMesh) return;
	if (!Owner->HasAuthority()) return;

	FVector Location = OwnerMesh->GetRelativeLocation();
	Location.X += _DeltaTime * FMath::Sin(StartTime * BobSpeed) * BobAmplitude.X;
	Location.Y += _DeltaTime * FMath::Sin(StartTime * BobSpeed) * BobAmplitude.Y;
	Location.Z += _DeltaTime * FMath::Sin(StartTime * BobSpeed) * BobAmplitude.Z;
	OwnerMesh->SetRelativeLocation(Location);

	StartTime += _DeltaTime;
	if (StartTime >= PI * 2)
	{
		StartTime = 0.0f;
	}
}

