// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaidialSpawner.generated.h"

UCLASS()
class PROTOTYPE2_API ARaidialSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaidialSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	AActor* initialSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> plot;

	UPROPERTY(EditAnywhere)
	int playercount = 12;

	void SetUp();
};
