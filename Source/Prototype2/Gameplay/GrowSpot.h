
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "NiagaraComponent.h"
#include "Prototype2/Pickups/Beehive.h"
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
	AGrowSpot();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	UPROPERTY(Replicated, EditAnywhere)
	int32 Player_ID = 0;

protected:
	virtual void BeginPlay() override;
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Plant();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem();

	void GrowPlantOnTick(float _DeltaTime);
	/**
	 * @brief play the mandrake noise if the player is holding a mandrake
	 * @param Player 
	 */
	void MandrakePickupNoise(APrototype2Character* _Player);
	/**
	 * @brief scale and position the Plant in the grow spot, called each frame to make it grow overtime
	 * @param Plant 
	 * @param TargetScale 
	 * @param PosOffset 
	 */
	void ScalePlant(APlant* _Plant, FVector _TargetScale, float _PosOffset) const;
public:	
	// Called every frame
	virtual void Tick(float _DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GrowOnTick(float _DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_UpdateState(EGrowSpotState _NewState);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlantReadySparkle(bool _bIsActive);
	
	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;

	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID) override;

	/* Planting from data asset called from Interact function */
	void PlantWeapon(APrototype2Character* _Player);
	void PlantPlant(APrototype2Character* _Player);
	
	/**
	 * @brief sets the plant and the variables for the plant to use for growing
	 * @param _Plant 
	 * @param _GrowTime 
	 */
	void SetPlant(APlant* _Plant, float _GrowTime);
	/**
	 * @brief sets the weapon and the variables from the weapon to be used for growing
	 * @param _Weapon 
	 * @param _GrowTime 
	 */
	void SetWeapon(AGrowableWeapon* _Weapon, float _GrowTime);
	/**
	 * @brief sets the beehive and the variables for the beehive
	 * @param _Weapon 
	 * @param _GrowTime 
	 */
	void SetBeehive(ABeehive* _Beehive, float _GrowTime);
	void MakePlantGold();

	UPROPERTY(Replicated, VisibleAnywhere)
	EGrowSpotState GrowSpotState = EGrowSpotState::Empty;

	UPROPERTY(Replicated, VisibleAnywhere)
	APlant* Plant = nullptr;
	UPROPERTY(Replicated, VisibleAnywhere)
	AGrowableWeapon* Weapon = nullptr;
	UPROPERTY(Replicated, VisibleAnywhere)
	ABeehive* Beehive = nullptr;

	UPROPERTY(Replicated, VisibleAnywhere)
	float GrowTimer{};
	
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTime{10};

	UPROPERTY(Replicated, EditAnywhere)
	bool bIsFertilised;

	UPROPERTY(Replicated, EditAnywhere)
	float FertiliseInteractDelay = 0.1;

	UPROPERTY(Replicated, EditAnywhere)
	float FertiliseInteractDelayTime;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ParticleSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* InteractSystem;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* MandrakeAttenuationSettings;

	// Plant ready sparkle VFX
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraSystem* PlantReadySparkle_NiagaraSystem;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* PlantReadySparkle_NiagaraComponent;

	/* Planting from PlantData Data Asset */
	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<APlant> PlantPrefab;

	/* Planting from WeaponData Data Asset */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGrowableWeapon> WeaponPrefab;
};
