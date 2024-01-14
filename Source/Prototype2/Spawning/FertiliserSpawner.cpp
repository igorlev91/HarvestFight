
#include "FertiliserSpawner.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Pickups/Fertiliser.h"

AFertiliserSpawner::AFertiliserSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InterfaceType = EInterfaceType::FertilizerBin;
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("Item Component"));
}

void AFertiliserSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);

	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	if (!HasAuthority())
		return;
	
	SpawnTimer = SpawnInterval;
}

void AFertiliserSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFertiliserSpawner, SpawnTimer);
	DOREPLIFETIME(AFertiliserSpawner, SpawnedFertiliser);
}

void AFertiliserSpawner::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!HasAuthority())
		return;

	GenerateFertiliserOnTimer(_DeltaTime);
}

UStaticMeshComponent* AFertiliserSpawner::GetMesh()
{
	return ItemComponent->Mesh;
}

void AFertiliserSpawner::Interact(APrototype2Character* _Player)
{
	if (!SpawnedFertiliser)
		return;

	SpawnTimer = SpawnInterval;
	
	_Player->PickupItem(SpawnedFertiliser);

	SpawnedFertiliser = nullptr;
}

void AFertiliserSpawner::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner,
	int _PlayerID)
{
	_InvokingWidget->SetHUDInteractText("Gather");
}

bool AFertiliserSpawner::IsInteractable(APrototype2PlayerState* _Player)
{
	if (SpawnTimer <= 0.0f)
		return true;
	else
		return false;
}

void AFertiliserSpawner::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);
}

void AFertiliserSpawner::GenerateFertiliserOnTimer(float _DeltaTime)
{
	if (SpawnTimer > 0)
	{
		SpawnTimer -= _DeltaTime;

		if (SpawnTimer <= 0)
		{
			SpawnFertiliser();
		}
	}
}

void AFertiliserSpawner::SpawnFertiliser()
{
	if (!HasAuthority())
		return;

	if (!FertiliserPrefab)
		return;

	SpawnedFertiliser = GetWorld()->SpawnActor<AFertiliser>(FertiliserPrefab);
	SpawnedFertiliser->SetReplicates(true);
	SpawnedFertiliser->ItemComponent->Mesh->SetSimulatePhysics(false);
	SpawnedFertiliser->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SpawnedFertiliser->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SpawnedFertiliser->ItemComponent->Multi_DisableCollisionAndAttach();
	SpawnedFertiliser->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SpawnedFertiliser->SetActorRelativeLocation({0.0f, 0.0f, 50.0f});
}

