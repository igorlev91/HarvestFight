// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Seed.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API ASeed : public APickUpItem
{
	GENERATED_BODY()
public:
	virtual void Interact(APrototype2Character* player) override;
	
public:
	UPROPERTY(EditAnywhere)
	float growtime;

	UPROPERTY(EditAnywhere)
	bool isPlanted;

	virtual void Grow();
};
