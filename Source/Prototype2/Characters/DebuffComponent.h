
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prototype2/DataAssets/PickupTypeEnum.h"
#include "DebuffComponent.generated.h"

USTRUCT()
struct FDebuffInfo
{
	GENERATED_BODY()
	UPROPERTY()
	EDebuff Debuff = EDebuff::None;
	UPROPERTY()
	float Duration = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE2_API UDebuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* Sets default values for this component's properties*/
	UDebuffComponent();

	/* For networking */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/* Called every frame*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void OnRep_ApplyDebuff();
	UFUNCTION(Server, Reliable)
	void Server_SetDebuff(EDebuff _InDebuff, float _Duration);
	
	/* Functionality for the daze debuff*/
	void UpdateDaze();

	/* Functionality for the punch debuff*/
	void Punch();

	void UpdatePunchCounter();

	/* Called until the debuff duration wears off*/
	UFUNCTION(Server, Reliable)
	void Server_RemoveDebuff();
	
	void DecrementTimers(float _DeltaTime);
	
	void ToggleDizzyVFX(bool _bTurnOn);

	UPROPERTY(VisibleAnywhere)
	EDebuff CurrentDebuff = EDebuff::None;
	UPROPERTY(VisibleAnywhere)
	float DebuffDuration;

	UPROPERTY(ReplicatedUsing=OnRep_ApplyDebuff)
	FDebuffInfo DebuffInfo;
protected:
	/* Called when the game starts*/
	virtual void BeginPlay() override;

private:
	class APrototype2Character* Player;
	

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float SlowSpeed = 300.0f;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float SlowAnimationRateScale = 0.7f;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float DazeSpeed = 400.0f;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float DazeAnimationRateScale = 0.9f;

	float PunchCounterDropOff = 2.5f;
	float PunchCounterDropOffTimer;

	UPROPERTY()
	float DazeRandomDirection = 0.0f;
	UPROPERTY()
	int PunchCounter = 0;

	float timer;
	float duration = 5.0f;
};
