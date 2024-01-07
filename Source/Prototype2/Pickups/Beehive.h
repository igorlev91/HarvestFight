#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Plant.h"
#include "Prototype2/DataAssets/BeehiveData.h"
#include "Beehive.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API ABeehive : public APlant
{
	GENERATED_BODY()
	// Functions
public:
	virtual void BeginPlay() override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;

	bool GetIfReady() const {return bIsReadyToCollect;}
private:

	// Variables
public:

private:
	UPROPERTY(VisibleAnywhere)
	int32 NumberOfNearbyFlowers;
	UPROPERTY(EditAnywhere)
	bool bIsReadyToCollect;

	UPROPERTY(EditAnywhere)
	UBeehiveData* BeehiveData;
};
