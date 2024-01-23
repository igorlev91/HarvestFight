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
	Mesh->SetIsReplicated(true);
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bDoBeginPlay)
	{
		return;
	}
	
	Mesh->SetCollisionProfileName("Ragdoll");
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);

	//if(GetOwner()->HasAuthority())
	//{
	//	//Mesh->SetSimulatePhysics(true);
	//}

	Mesh->SetRenderCustomDepth(false);
}

void UItemComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
}

void UItemComponent::Interact(APrototype2Character* _Player, APickUpItem* _ItemPickedUp)
{
	_Player->PickupItem(_ItemPickedUp, _ItemPickedUp->PickupActor);
	_Player->HeldItem = _ItemPickedUp;
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemComponent, Mesh);
	DOREPLIFETIME(UItemComponent, bGold);
	DOREPLIFETIME(UItemComponent, bDoBeginPlay);
}

void UItemComponent::InitializeSeed(TArray<UMaterialInstance*> _InMaterials, UStaticMesh* _InMesh)
{
	if (_InMesh)
	{
		Mesh->SetStaticMesh(_InMesh);
	}
	for (int i = 0; i < _InMaterials.Num(); i++)
	{
		Mesh->SetMaterial(i, _InMaterials[i]);
	}
}

void UItemComponent::Multi_DisableCollisionAndAttach_Implementation()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

