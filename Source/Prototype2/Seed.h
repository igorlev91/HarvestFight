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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Interact(APrototype2Character* player) override;
	virtual void ClientInteract(APrototype2Character* player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* player) override;
public:

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleParachuteVisibility(bool _visible);
	void Multi_ToggleParachuteVisibility_Implementation(bool _visible);

	UPROPERTY(EditAnywhere)
	class UStaticMesh* WinterParachute;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* NormalParachute;
	
	UPROPERTY(EditAnywhere)
	float BobSpeed{120.0f};

	UPROPERTY(EditAnywhere)
	float BobAmplitude{8.0f};
	
	UPROPERTY(EditAnywhere)
	float DropDistance{400};

	UPROPERTY(EditAnywhere)
	float FallTime{3.0f};

	UPROPERTY(VisibleAnywhere)
	float SpawnTime;

	UFUNCTION()
	void HandleParachuteMovement();

	UPROPERTY(VisibleAnywhere)
	FVector SpawnPos;
	UPROPERTY(VisibleAnywhere)
	FRotator SpawnRotation;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ParachuteMesh;
	
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
