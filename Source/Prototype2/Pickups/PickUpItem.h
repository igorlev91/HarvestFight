 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "ItemComponent.h"
#include "Prototype2/DataAssets/PlantData.h"
#include "Prototype2/DataAssets/WeaponData.h"
#include "PickUpItem.generated.h"

/* Distinguish between seed/plant/weapon */
UENUM()
enum class EPickupActor : uint8
{
	Default = 0,
	WeaponActor,
	PlantActor,
	SeedActor,
	FertilizerActor,
	BeehiveActor,
};
class APrototype2Character;
UCLASS()
class PROTOTYPE2_API APickUpItem : public AActor
{
	GENERATED_BODY()
	
public:	
	/* Sets default values for this actor's properties */
	APickUpItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

	virtual void Client_Pickup();
	
	virtual void Client_Drop();
	virtual void Server_Drop() {}

	/* Called from SeedSpawner to give seed a specific data asset and setup material */
	void SetSeedData(class USeedData* _Data, EPickupActor _PickupType);

	UFUNCTION(Server, Reliable)
	void Server_SetSeedData(USeedData* _Data, EPickupActor _PickupType);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetSeedData(USeedData* _Data, EPickupActor _PickupType);
	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;

	UPROPERTY(EditAnywhere)
	class USeedData* SeedData{nullptr};

	// Todo: Rename to PickupType when other Enum is replaced
	UPROPERTY(VisibleAnywhere)
	EPickupDataType DataAssetPickupType; //to replace pickup type in itemcomponent

	UPROPERTY(EditAnywhere)
	EPickupActor PickupActor;

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
