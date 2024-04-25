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

USTRUCT()
struct FServerData
{
	GENERATED_BODY()
 	
	UPROPERTY(EditAnywhere)
	class USeedData* SeedData{nullptr};

	UPROPERTY(EditAnywhere)
	EPickupActor PickupActor = EPickupActor::SeedActor;
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

	UFUNCTION()
	void OnRep_ServerData(FServerData& _Data);
	
	UFUNCTION()
	void GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData);

	UFUNCTION()
	void GoldChanged();
	
	UPROPERTY()
	float GetHitMultiplier = 10.0f;
	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UItemComponent* ItemComponent;

	UPROPERTY(VisibleAnywhere)
	class USquashAndStretch* SSComponent;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_ServerData)
	FServerData ServerData;

	UPROPERTY(VisibleAnywhere)
	bool bInitialized{};
	
	UPROPERTY(EditAnywhere, Category="SFX")
	UAudioComponent* GoldSoundComponent;
protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
