

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Fertiliser.generated.h"

UCLASS()
class PROTOTYPE2_API AFertiliser : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()

public:
	AFertiliser();
	virtual void Destroyed() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode = INVALID) override;

	// Wilting
public:
	UFUNCTION()
	void Wilt(float DeltaTime);
	UFUNCTION()
	void WiltMaterial();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnInteract();
	virtual void Server_Drop() override;
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnDestroy();

	UPROPERTY(VisibleAnywhere)
	TArray<class UMaterialInstanceDynamic*> Materials;
	
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
};
