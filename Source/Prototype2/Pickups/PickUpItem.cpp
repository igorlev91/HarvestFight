// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/VFX/SquashAndStretch.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	//SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void APickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpItem, ItemComponent);
	DOREPLIFETIME(APickUpItem, SeedData);
	DOREPLIFETIME(APickUpItem, PickupActor);
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (ItemComponent->Mesh->GetNumMaterials() == 0 && IsValid(SeedData))
	{
		switch(PickupActor)
		{
		case EPickupActor::SeedActor:
			{
				ItemComponent->InitializeSeed(SeedData->PacketMaterials, SeedData->PacketMesh);
				break;
			}
		case EPickupActor::FertilizerActor:
			{
				ItemComponent->InitializeSeed(SeedData->PacketMaterials, SeedData->PacketMesh);
				break;
			}
		default:
			{
				ItemComponent->InitializeSeed(SeedData->BabyMaterials, SeedData->BabyMesh);
				break;
			}
		}
	}
}

void APickUpItem::Client_Pickup()
{
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	//SSComponent->Boing();
}

void APickUpItem::Client_Drop()
{
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	//SSComponent->Boing();
}

void APickUpItem::SetSeedData(USeedData* _Data, EPickupActor _PickupType)
{
	if (HasAuthority())
	{
		PickupActor = _PickupType;
		SeedData = _Data;

		switch(PickupActor)
		{
		case EPickupActor::SeedActor:
			{
				ItemComponent->InitializeSeed(_Data->PacketMaterials, _Data->PacketMesh);
				break;
			}
		case EPickupActor::FertilizerActor:
			{
				ItemComponent->InitializeSeed(_Data->PacketMaterials, _Data->PacketMesh);
				break;
			}
		default:
			{
				ItemComponent->InitializeSeed(_Data->BabyMaterials, _Data->BabyMesh);
				break;
			}
		}
	}
}

void APickUpItem::OnRep_SetSeedData(USeedData* _Data)
{
	if (!IsValid(_Data))
		return;
	
	switch(PickupActor)
	{
	case EPickupActor::SeedActor:
		{
			ItemComponent->InitializeSeed(_Data->PacketMaterials, _Data->PacketMesh);
			break;
		}
	case EPickupActor::FertilizerActor:
		{
			ItemComponent->InitializeSeed(_Data->PacketMaterials, _Data->PacketMesh);
			break;
		}
	default:
		{
			ItemComponent->InitializeSeed(_Data->BabyMaterials, _Data->BabyMesh);
			break;
		}
	}
}

void APickUpItem::OnRep_SetPickupActor(EPickupActor _Type)
{
	if (!IsValid(SeedData))
		return;
	
	switch(_Type)
	{
	case EPickupActor::SeedActor:
		{
			ItemComponent->InitializeSeed(SeedData->PacketMaterials, SeedData->PacketMesh);
			break;
		}
	case EPickupActor::FertilizerActor:
		{
			ItemComponent->InitializeSeed(SeedData->PacketMaterials, SeedData->PacketMesh);
			break;
		}
	default:
		{
			ItemComponent->InitializeSeed(SeedData->BabyMaterials, SeedData->BabyMesh);
			break;
		}
	}
}


