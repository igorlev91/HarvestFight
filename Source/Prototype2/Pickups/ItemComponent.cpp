#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Net/UnrealNetwork.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	if(GetOwner())
	{
		GetOwner()->SetRootComponent(Mesh);
	}

	Mesh->SetRenderCustomDepth(false);
	Mesh->CustomDepthStencilValue = 1;
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh->SetCollisionProfileName("Ragdoll");
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);

	if(GetOwner()->HasAuthority())
	{
		Mesh->SetSimulatePhysics(true);
	}

	Mesh->SetRenderCustomDepth(false);
}

void UItemComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
}

void UItemComponent::Interact(APrototype2Character* _Player, APickUpItem* _ItemPickedUp)
{
	_Player->Server_PickupItem(this, _ItemPickedUp);
	_Player->HeldItem = _ItemPickedUp;
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemComponent, bGold);
	DOREPLIFETIME(UItemComponent, bIsGold);
	DOREPLIFETIME(UItemComponent, PickupType);
}

void UItemComponent::InitializeSeed(UMaterialInstance* _InMaterial)
{
	if (_InMaterial)
	{
		Mesh->SetMaterial(0, _InMaterial);
	}
}

void UItemComponent::InitializePlant(UStaticMesh* _InMesh)
{
	if (_InMesh)
	{
		Mesh->SetStaticMesh(_InMesh);
	}
}

void UItemComponent::InitializeWeapon(UStaticMesh* _InMesh)
{
	if (_InMesh)
	{
		Mesh->SetStaticMesh(_InMesh);
	}
}

