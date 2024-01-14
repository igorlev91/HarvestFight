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
	
	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

	/* Called from SeedSpawner to give seed a specific data asset and setup material */
	void SetPlantSeedData(UPlantData* _Data);
	void SetWeaponSeedData(UWeaponData* _Data);
	
	/* Called from GrowSpot to give plant a specific data asset and setup static mesh */
	void SetPlantData(UPlantData* _Data);
	void SetWeaponData(UWeaponData* _Data);

	UFUNCTION(Server, Reliable)
	void Server_SetPlantSeedData(UPlantData* _Data);
	UFUNCTION(Server, Reliable)
	void Server_SetWeaponSeedData(UWeaponData* _Data);
	UFUNCTION(Server, Reliable)
	void Server_SetPlantData(UPlantData* _Data);
	UFUNCTION(Server, Reliable)
	void Server_SetWeaponData(UWeaponData* _Data);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlantSeedData(UPlantData* _Data);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetWeaponSeedData(UWeaponData* _Data);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlantData(UPlantData* _Data);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetWeaponData(UWeaponData* _Data);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;

	UPROPERTY(VisibleAnywhere)
	UPlantData* PlantData = nullptr;
	UPROPERTY(VisibleAnywhere)
	UWeaponData* WeaponData = nullptr;

	// Todo: Rename to PickupType when other Enum is replaced
	UPROPERTY(VisibleAnywhere)
	EPickupDataType DataAssetPickupType; //to replace pickup type in itemcomponent

	UPROPERTY(VisibleAnywhere)
	EPickupActor PickupActor;

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
