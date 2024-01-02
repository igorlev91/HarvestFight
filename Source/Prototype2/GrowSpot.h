// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "ItemComponent.h"
#include "GrowSpot.generated.h"

class APlant;
class AGrowableWeapon;
class ASeed;
class AWeaponSeed;
UCLASS()
class PROTOTYPE2_API AGrowSpot : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrowSpot();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	int Player_ID = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Plant();
	void Multi_Plant_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem();
	void Multi_FireParticleSystem_Implementation();

	void GrowPlantOnTick(float DeltaTime);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;

	virtual void Interact(APrototype2Character* player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) override;

	void SetPlant(APlant* _plant, float _growTime);
	void SetWeapon(AGrowableWeapon* _weapon, float _growTime);

	UPROPERTY(Replicated, VisibleAnywhere)
	EGrowSpotState GrowSpotState = EGrowSpotState::Default;

	UPROPERTY(Replicated, VisibleAnywhere)
	APlant* plant = nullptr;
	UPROPERTY(Replicated, VisibleAnywhere)
	AGrowableWeapon* weapon = nullptr;

	UPROPERTY(Replicated, VisibleAnywhere)
	float growTimer{};
	
	UPROPERTY(Replicated, EditAnywhere)
	float growTime{10};

	UPROPERTY(Replicated, VisibleAnywhere)
	bool growingPlant = false;
	UPROPERTY(Replicated, VisibleAnywhere)
	bool plantGrown = false;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ParticleSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* InteractSystem;
};
