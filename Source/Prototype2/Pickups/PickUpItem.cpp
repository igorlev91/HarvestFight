// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/PlantData.h"
#include "Prototype2/VFX/SquashAndStretch.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUpItem::SetPlantSeedData(UPlantData* _Data)
{
	Server_SetPlantSeedData(_Data);
}

void APickUpItem::SetWeaponSeedData(UWeaponData* _Data)
{
	Server_SetWeaponSeedData(_Data);
}

void APickUpItem::SetPlantData(UPlantData* _Data)
{
	Server_SetPlantData(_Data);
}

void APickUpItem::SetWeaponData(UWeaponData* _Data)
{
	Server_SetWeaponData(_Data);
}

void APickUpItem::Multi_SetPlantSeedData_Implementation(UPlantData* _Data)
{
	PlantData = _Data;
	PickupActor = EPickupActor::SeedActor;
	DataAssetPickupType = EPickupDataType::PlantData;
	ItemComponent->InitializeSeed(_Data->SeedMaterial);
}

void APickUpItem::Multi_SetWeaponSeedData_Implementation(UWeaponData* _Data)
{
	WeaponData = _Data;
	PickupActor = EPickupActor::SeedActor;
	DataAssetPickupType = EPickupDataType::WeaponData;
	ItemComponent->InitializeSeed(_Data->SeedMaterial);
}

void APickUpItem::Multi_SetPlantData_Implementation(UPlantData* _Data)
{
	PlantData = _Data;
	PickupActor = EPickupActor::PlantActor;
	DataAssetPickupType = EPickupDataType::PlantData;
	ItemComponent->InitializePlant(_Data->PlantMesh);
}

void APickUpItem::Multi_SetWeaponData_Implementation(UWeaponData* _Data)
{
	WeaponData = _Data;
	PickupActor = EPickupActor::WeaponActor;
	DataAssetPickupType = EPickupDataType::WeaponData;
	ItemComponent->InitializeWeapon(_Data->WeaponMesh);
}

void APickUpItem::Server_SetPlantSeedData_Implementation(UPlantData* _Data)
{
	Multi_SetPlantSeedData(_Data);
}

void APickUpItem::Server_SetWeaponSeedData_Implementation(UWeaponData* _Data)
{
	Multi_SetWeaponSeedData(_Data);
}

void APickUpItem::Server_SetPlantData_Implementation(UPlantData* _Data)
{
	Multi_SetPlantData(_Data);
}

void APickUpItem::Server_SetWeaponData_Implementation(UWeaponData* _Data)
{
	Multi_SetWeaponData(_Data);
}


