// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Widgets/Widget_PlayerHUD.h"
#include "ItemComponent.generated.h"


class APrototype2Character;
class APickUpItem;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE2_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void Interact(APrototype2Character* player, APickUpItem* itemPickedUp);
	
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere)
	float CropValue;
	
	UPROPERTY(EditAnywhere)
	int32 WeaponDurability;

	// For HUD changing icons
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EPickup> PickupType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrowTime;	
};
