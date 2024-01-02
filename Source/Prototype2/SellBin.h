// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "ItemComponent.h"
#include "SellBin.generated.h"

class UWidgetComponent;
class APrototype2Character;
class APlant;
UCLASS()
class PROTOTYPE2_API ASellBin : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASellBin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(Server, Reliable)
    void Server_FireParticleSystem();
    void Server_FireParticleSystem_Implementation();
	void FireSellFX(APlant* _plant, APrototype2Character* player);
	UFUNCTION(Client, Reliable)
	void Client_MoveUI(float DeltaTime);
	void Client_MoveUI_Implementation(float DeltaTime);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem();
	void Multi_FireParticleSystem_Implementation();
	
	void HideParticleSystem();

	virtual void ClientInteract(APrototype2Character* player) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;

	// Selling UI
	UPROPERTY(EditAnywhere)
	UWidgetComponent* SellAmountWidgetComponent;
	UPROPERTY(EditAnywhere)
	FVector startPosition{};
	UPROPERTY(VisibleAnywhere)
	bool isMoving{};
	UPROPERTY(EditAnywhere)
	float moveSpeed{};
	UPROPERTY(EditAnywhere)
	float movingTime{};
	UPROPERTY(VisibleAnywhere)
	float movingTimer{};
	void MoveUIComponent(float _dt);

	UPROPERTY(Replicated)
	bool bWidgetVisible{};

	

	virtual void Interact(APrototype2Character* player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) override;
private:

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ParticleSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* InteractSystem;
};
