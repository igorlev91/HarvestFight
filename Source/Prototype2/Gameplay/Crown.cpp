
#include "Crown.h"

#include "BobTransformComponent.h"
#include "Net/UnrealNetwork.h"

ACrown::ACrown()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	RootTransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootTransformComponent"));
	RootComponent = RootTransformComponent;
	RootTransformComponent->SetIsReplicated(true);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetIsReplicated(true);
	BobTransformComponent = CreateDefaultSubobject<UBobTransformComponent>(TEXT("Bob Component"));
}

void ACrown::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	if (HasAuthority())
		BobTransformComponent->SetEnabled(true);
}

void ACrown::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrown, Mesh);
	DOREPLIFETIME(ACrown, RootTransformComponent);
}

void ACrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* ACrown::GetMesh()
{
	return Mesh;
}

