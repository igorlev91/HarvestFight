
#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Plant.generated.h"

UCLASS()
class PROTOTYPE2_API APlant : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()
public:
	APlant();
	virtual void BeginPlay() override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void HoldInteract(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Server_Drop() override;
	virtual void Client_Drop() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void Wilt(float DeltaTime);
	UFUNCTION()
	void WiltMaterial();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnInteract();
	UFUNCTION(Server, Reliable)
	void Server_Destroy();
	
	void Multi_ScalePlant();

	bool bGrown = false;
	UPROPERTY(EditAnywhere)
	int32 NumberOfNearbyFlowers{};

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> DestroyVFX{};
	
	UPROPERTY(EditAnywhere)
	float InitialLifetime{30};
	UPROPERTY(VisibleAnywhere)
	float Lifetime{};
	
	UPROPERTY(EditAnywhere)
	float WiltDelay{30};
	UPROPERTY(VisibleAnywhere)
	float WiltDelayTimer{};

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bShouldWilt{};

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bPoisoned{};

	UPROPERTY(VisibleAnywhere)
	TArray<class UMaterialInstanceDynamic*> PlantMats;
};
