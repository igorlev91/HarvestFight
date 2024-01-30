
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prototype2/DataAssets/PickupTypeEnum.h"
#include "DebuffComponent.generated.h"

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

	/* Called by GetHit() functions, applies whatever debuff passed in */
	void ApplyDebuff(EDebuff _DebuffType, float _Duration);
	
	/* Functionality for the daze debuff*/
	void UpdateDaze();

	/* Functionality for the punch debuff*/
	void Punch();
	UFUNCTION(Server, Reliable)
	void Server_UpdatePunchCounter();

	/* Called until the debuff duration wears off*/
	UFUNCTION(Server, Reliable)
	void Server_RemoveDebuff();
	
	void DecrementTimers(float _DeltaTime);

	UFUNCTION(Client, Reliable)
	void Client_ToggleDizzyVFX(bool _bTurnOn);
	
	UPROPERTY(Replicated)
	EDebuff CurrentDebuff = EDebuff::None;
protected:
	/* Called when the game starts*/
	virtual void BeginPlay() override;

private:
	class APrototype2Character* Player;
	
	float DebuffDuration;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float SlowSpeed = 300.0f;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float SlowAnimationRateScale = 0.7f;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float DazeSpeed = 400.0f;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	float DazeAnimationRateScale = 0.9f;
	UPROPERTY(Replicated)
	float DazeRandomDirection = 0.0f;

	float PunchCounterDropOff = 2.5f;
	float PunchCounterDropOffTimer;
	UPROPERTY(Replicated)
	int PunchCounter = 0;
};
