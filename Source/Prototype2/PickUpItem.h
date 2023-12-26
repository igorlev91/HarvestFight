// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "ItemComponent.h"
#include "PickUpItem.generated.h"

UCLASS()
class PROTOTYPE2_API APickUpItem : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*	get character
		attach mesh to characters hand
		set characters HeldItem to this */
	virtual void Interact(){}

public:
	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;

};
