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

	SetIsReplicatedByDefault(true);
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void UItemComponent::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
}

void UItemComponent::Interact(APrototype2Character* _Player, APickUpItem* _ItemPickedUp)
{
	_Player->PickupItem(_ItemPickedUp, _ItemPickedUp->ServerData.PickupActor);
	_Player->HeldItem = _ItemPickedUp;
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemComponent, bGold);
	DOREPLIFETIME(UItemComponent, PlayerWhoThrewItem);
}

void UItemComponent::InitializeSeed(TArray<UMaterialInstance*> _InMaterials, UStaticMesh* _InMesh)
{
	if (IsValid(_InMesh))
	{
		Mesh->SetStaticMesh(_InMesh);
	}
	
	for (int i = 0; i < _InMaterials.Num(); i++)
	{
		if (IsValid(_InMaterials[i]))
		{
			Mesh->SetMaterial(i, _InMaterials[i]);
			Mesh->MarkRenderStateDirty();
		}
	}
}

void UItemComponent::SetStencilEnabled(bool _StencilEnabled)
{
	Mesh->SetRenderCustomDepth(_StencilEnabled);
}

void UItemComponent::OnRep_bGold()
{
	if (bGold == true)
	{
		auto MyOwner = GetOwner();
		if (!IsValid(MyOwner))
			return;
		auto CastedOwner = Cast<APickUpItem>(MyOwner);
		if (!IsValid(CastedOwner))
			return;
		if (!IsValid(CastedOwner->ServerData.SeedData))
			return;
		
		for (int i = 0; i < Mesh->GetNumMaterials(); i++)
		{
			if (CastedOwner->ServerData.SeedData->BabyGoldMaterials.Num() > i)
				Mesh->SetMaterial(i, CastedOwner->ServerData.SeedData->BabyGoldMaterials[i]);
			else if (CastedOwner->ServerData.SeedData->BabyGoldMaterials.Num() > 0)
				Mesh->SetMaterial(i, CastedOwner->ServerData.SeedData->BabyGoldMaterials[0]);
			bInitialized = true;
		}
	}
	else
	{
		auto MyOwner = GetOwner();
		if (!IsValid(MyOwner))
			return;
		auto CastedOwner = Cast<APickUpItem>(MyOwner);
		if (!IsValid(CastedOwner))
			return;
		if (!IsValid(CastedOwner->ServerData.SeedData))
			return;
		
		for (int i = 0; i < Mesh->GetNumMaterials(); i++)
		{
			if (CastedOwner->ServerData.SeedData->BabyMaterials.Num() > i)
				Mesh->SetMaterial(i, CastedOwner->ServerData.SeedData->BabyMaterials[i]);
			else if (CastedOwner->ServerData.SeedData->BabyMaterials.Num() > 0)
				Mesh->SetMaterial(i, CastedOwner->ServerData.SeedData->BabyMaterials[0]);
		}
	}

	/* Toggle GOLD sound */
	OnGoldChangedDelegate.Broadcast();
}

void UItemComponent::Multi_DisableCollisionAndAttach_Implementation()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

