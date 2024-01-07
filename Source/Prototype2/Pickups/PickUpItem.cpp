// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/PlantData.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
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
	PlantData = _Data;
	ItemComponent->InitializeSeed(_Data->SeedMaterial);
	PickupActor = EPickupActor::SeedActor;
	DataAssetPickupType = EPickupDataType::PlantData;
}

void APickUpItem::SetWeaponSeedData(UWeaponData* _Data)
{
	WeaponData = _Data;
	ItemComponent->InitializeSeed(_Data->SeedMaterial);
	PickupActor = EPickupActor::SeedActor;
	DataAssetPickupType = EPickupDataType::WeaponData;
}

void APickUpItem::SetPlantData(UPlantData* _Data)
{
	PlantData = _Data;
	ItemComponent->InitializePlant(_Data->PlantMesh);
	PickupActor = EPickupActor::PlantActor;
	DataAssetPickupType = EPickupDataType::PlantData;
}

void APickUpItem::SetWeaponData(UWeaponData* _Data)
{
	WeaponData = _Data;
	ItemComponent->InitializeWeapon(_Data->WeaponMesh);
	PickupActor = EPickupActor::WeaponActor;
	DataAssetPickupType = EPickupDataType::WeaponData;
}
