
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
	void Multi_Wilt(float DeltaTime);
	void Multi_ScalePlant();
	void Multi_SetWilt(bool _bShouldWilt);

	bool bGrown = false;
	UPROPERTY(EditAnywhere)
	int32 NumberOfNearbyFlowers = 0;

	// Time to wait after being dropped until wilting starts
	static constexpr float WiltingWaitTime{5};

	UPROPERTY(EditAnywhere, Replicated)
	float WiltingDelayTimer{0};

	UPROPERTY(EditAnywhere, Replicated)
	float WiltTime{30};

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bShouldWilt = false;
};
