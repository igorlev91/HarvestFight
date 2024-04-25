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
	bReplicates = true;
	
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Item Squash and stretch"));
	
	GoldSoundComponent = CreateDefaultSubobject<UAudioComponent>("GoldAudioCompoent");
	GoldSoundComponent->SetupAttachment(RootComponent);
}

void APickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpItem, ItemComponent);
	DOREPLIFETIME(APickUpItem, ServerData);
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	SSComponent->OwningItem = this;

	ItemComponent->OnGoldChangedDelegate.AddDynamic(this, &APickUpItem::GoldChanged);
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bInitialized && IsValid(ServerData.SeedData))
	{
		OnRep_ServerData(ServerData);
	}
}

void APickUpItem::Client_Pickup()
{
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	SSComponent->Disable();
}

void APickUpItem::Client_Drop()
{
	ItemComponent->Mesh->SetRenderCustomDepth(false);
}

void APickUpItem::SetSeedData(USeedData* _Data, EPickupActor _PickupType)
{
	ServerData.SeedData = _Data;
	ServerData.PickupActor = _PickupType;

	switch(_PickupType)
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

void APickUpItem::OnRep_ServerData(FServerData& _Data)
{
	if (!IsValid(_Data.SeedData))
		return;
	
	switch(_Data.PickupActor)
	{
	case EPickupActor::SeedActor:
		{
			ItemComponent->InitializeSeed(_Data.SeedData->PacketMaterials, _Data.SeedData->PacketMesh);
			break;
		}
	case EPickupActor::FertilizerActor:
		{
			ItemComponent->InitializeSeed(_Data.SeedData->PacketMaterials, _Data.SeedData->PacketMesh);
			break;
		}
	default:
		{
			ItemComponent->InitializeSeed(_Data.SeedData->BabyMaterials, _Data.SeedData->BabyMesh);
			break;
		}
	}


	bInitialized = true;
}


void APickUpItem::GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData)
{
	// Removing for beta submission
	return;
	// Knockback
	FVector MakeItemSameZValue = _AttackerLocation;
	MakeItemSameZValue.Z = GetActorLocation().Z;
	FVector KnockAway = (GetActorUpVector() * _OtherWeaponData->KnockUpMultiplier) + (GetActorLocation() - MakeItemSameZValue).GetSafeNormal();
	
	// Set minimum attack charge for scaling knockback
	if (_AttackCharge < 1.0f)
	{
		_AttackCharge = 1.0f;
	}
	
	KnockAway *= _AttackCharge * _OtherWeaponData->KnockbackMultiplier;
	
	// Limit the knockback to MaxKnockBackVelocity
	if (KnockAway.Size() > _OtherWeaponData->MaxKnockback) 
	{
		KnockAway = KnockAway.GetSafeNormal() * _OtherWeaponData->MaxKnockback; 
	}

	ItemComponent->Mesh->AddImpulse(KnockAway * GetHitMultiplier);
	UKismetSystemLibrary::PrintString(GetWorld(),"Hit Item");
}

void APickUpItem::GoldChanged()
{
	if (ServerData.PickupActor != EPickupActor::PlantActor)
		return;

	if (!GoldSoundComponent)
		return;
	
	if (!ItemComponent)
		return;

	if (ItemComponent->bGold)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Played Gold Sound");
		GoldSoundComponent->Play();
	}
	else
	{
		GoldSoundComponent->Stop();
	}
}