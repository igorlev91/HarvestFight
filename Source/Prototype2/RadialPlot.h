// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrowSpot.h"
#include "GameFramework/Actor.h"
#include "RadialPlot.generated.h"

UCLASS()
class PROTOTYPE2_API ARadialPlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadialPlot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGrowSpot*> growSpots;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPlayerID(int ID);

};
