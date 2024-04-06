

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RandomEventManager.generated.h"

UENUM(BlueprintType)
enum ERandomEvent
{
	DOUBLE_POINTS = 0,
	ALL_GOLD,
	INSTANT_GROW,
	RAINING_SEEDS,
	/*RANDOM_GROWTH,
	PLANT_MUTATION,*/

	END
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE2_API URandomEventManager : public UActorComponent
{
	friend class APrototype2GameMode;
	GENERATED_BODY()

protected:
	URandomEventManager();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TriggerEvent(ERandomEvent _Event);

	UFUNCTION(BlueprintCallable)
	void TriggerDoublePoints();
	UFUNCTION(BlueprintCallable)
	void TriggerAllGold();
	UFUNCTION(BlueprintCallable)
	void TriggerInstantGrow();
	UFUNCTION(BlueprintCallable)
	void TriggerRainingSeeds();
	UFUNCTION(BlueprintCallable)
	void TriggerRandomGrowth();
	UFUNCTION(BlueprintCallable)
	void TriggerPlantMutation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_MakePlantsGold();

public:
	UFUNCTION()
	int8 GetSellMultiplier() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ERandomEvent> ActiveEvent{ ERandomEvent::DOUBLE_POINTS };
	
	UPROPERTY(EditDefaultsOnly)
	float RandomEventInterval{30.0f};

	UPROPERTY(EditDefaultsOnly)
	int8 RandomEventIntervalVarience{15};

	UPROPERTY(VisibleAnywhere)
	float RandomEventTimer{};

	UPROPERTY(VisibleAnywhere)
	bool bHasTriggeredDoublePoints{};

	UPROPERTY(EditDefaultsOnly)
	float RandomEventDuration{30.0f};
	
	UPROPERTY(VisibleAnywhere)
	float RandomEventDurationTimer{};

	UPROPERTY(VisibleAnywhere)
	class APrototype2GameMode* GamemodeOwner;
	UPROPERTY(VisibleAnywhere)
	class APrototype2Gamestate* GameStateRef;

	UPROPERTY(EditDefaultsOnly, Category="Event Properties")
	int8 ChanceOfRandomGrowthOnPlot{2};
	
	UPROPERTY(EditDefaultsOnly, Category="Event Properties")
	int8 SeedRainInterval{5};
};
