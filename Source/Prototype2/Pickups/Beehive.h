
#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Pickups/Plant.h"
#include "Beehive.generated.h"

class AGrowSpot;
/**
 * 
 */
UCLASS()
class PROTOTYPE2_API ABeehive : public APlant
{
	GENERATED_BODY()
	// Functions
public:
	ABeehive();
	
	virtual void BeginPlay() override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	void SetBeehiveLocation(FVector _Location);

	UFUNCTION(Server, reliable)
	void Server_SetBeehiveLocation(FVector _Location);
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DestroyBees();

	// Variables
public:
	UPROPERTY(Replicated, VisibleAnywhere)
	AGrowSpot* ParentGrowSpot{};
	
	UPROPERTY(EditAnywhere)
	float FlowerEffectionDistance{600.0f};
	
	UPROPERTY(EditAnywhere)
	UPlantData* HoneyData;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	bool bIsReadyToCollect = false;

	UPROPERTY(EditAnywhere)
	bool bCollectOverTime;

	UPROPERTY(EditAnywhere)
	float TimeTillCollect = 15.0f;
	UPROPERTY(Replicated, VisibleAnywhere)
	float TrackerTimeTillCollect;

	UPROPERTY(EditAnywhere)
	int MaxHarvestableHoney = 3;
	UPROPERTY(Replicated, VisibleAnywhere)
	int HarvestedHoney;

	// Bees
	UPROPERTY(VisibleAnywhere)
	class AFlock* Bees;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFlock> BeesPrefab;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APlant> HoneyPrefab;
};
