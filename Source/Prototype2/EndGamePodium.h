// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndGamePodium.generated.h"

UCLASS()
class PROTOTYPE2_API AEndGamePodium : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndGamePodium();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* P1LosePosition;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* P2LosePosition;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* P3LosePosition;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* P4LosePosition;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* P1WinPosition;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* P2WinPosition;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* P3WinPosition;
	UPROPERTY(EditAnywhere)
	class UArrowComponent* P4WinPosition;

	UPROPERTY(Transient, Replicated, EditAnywhere)
	class AEndGameCamera* EndGameCamera;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEndGameCamera> EndGameCameraPrefab;
	
	//UPROPERTY(EditAnywhere)
	//TArray<class UStaticMeshComponent*> FirstPlaceMeshes;
	//UPROPERTY(EditAnywhere)
	//TArray<class UStaticMeshComponent*> SecondPlaceMeshes;
	//UPROPERTY(EditAnywhere)
	//TArray<class UStaticMeshComponent*> ThirdPlaceMeshes;

	UPROPERTY(EditAnywhere)
	TArray<class UStaticMeshComponent*> PodiumMeshes;
};
