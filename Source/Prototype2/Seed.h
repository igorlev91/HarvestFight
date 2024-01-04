// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Seed.generated.h"

/**
 * 
 */
class APlant;
UCLASS()
class PROTOTYPE2_API ASeed : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()


public:
	ASeed();
	virtual void Interact(APrototype2Character* player) override;
	virtual void ClientInteract(APrototype2Character* player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) override;
	
public:
	UPROPERTY(EditAnywhere)
	float growtime;

	UPROPERTY(EditAnywhere)
	bool isPlanted;

	UPROPERTY(EditAnywhere)
	bool isWeapon;

	virtual void Grow();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> plantToGrow;
};
