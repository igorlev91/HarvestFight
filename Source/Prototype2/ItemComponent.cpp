// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemComponent.h"
#include "Prototype2Character.h"
#include "PickUpItem.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UItemComponent::UItemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	if(GetOwner())
	{
		GetOwner()->SetRootComponent(Mesh);
	}
}

// Called when the game starts
void UItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup Collisions and physics
	Mesh->SetCollisionProfileName("Ragdoll");
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	
	if (Mesh->Mobility.GetValue() == EComponentMobility::Movable)
	{
		Mesh->SetSimulatePhysics(true);
		//Mesh->BodyInstance.bLockXRotation = true;
		//Mesh->BodyInstance.bLockYRotation = true;
		Mesh->BodyInstance.bLockXTranslation = true;
		Mesh->BodyInstance.bLockYTranslation = true;
	}
}

// Called every frame
void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemComponent::Interact(APrototype2Character* player, APickUpItem* itemPickedUp)
{
	// Setup Mesh
	player->Server_PickupItem(this, itemPickedUp);

	// Debug
	//UE_LOG(LogTemp, Warning, TEXT("HeldItem attached to hand"));	
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemComponent, gold);
	DOREPLIFETIME(UItemComponent, bIsGold);
}

