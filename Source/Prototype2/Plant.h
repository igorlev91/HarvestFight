// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Plant.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API APlant : public APickUpItem
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	float value;
};
