
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "NiagaraComponent.h"
#include "RadialPlot.h"
#include "Components/TimelineComponent.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Pickups/Beehive.h"
#include "GrowSpot.generated.h"

USTRUCT()
struct FFertilisationState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	bool bFertilised = false;
	
	UPROPERTY(VisibleAnywhere)
	int32 ConcretedHealth = 0;
};

class APlant;
class AGrowableWeapon;
class ASeed;
class AWeaponSeed;
UCLASS()
class PROTOTYPE2_API AGrowSpot : public AActor, public IInteractInterface
{
	GENERATED_BODY()

protected:
	friend class URandomEventManager;
	
	/* INITIALIZATION */
	AGrowSpot();
	void InitComponents();
	void InitAssignableVariables();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	/* ACTOR OVERRIDES */
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	
public:
	/* INTERACTION */
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player) override;
	EInteractMode IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete = true);
	EInteractMode IsInteractable_Stealing(APrototype2PlayerState* _Player);
	
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void Interact(APrototype2Character* _Player) override;
	
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID) override;
	void OnDisplayInteractText_Unprotected(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner);
	
	/* PLANT/GROW CONTROL */
	void PlantASeed(ASeed* _SeedToPlant);
	void PlantASeed(APickUpItem* _SeedToPlant);
	
	void GrowPlantOnTick(float _DeltaTime);
	void CompleteGrowth();
	void DestroyPlant();

	/* CONCRETE */
	bool DegradeConcrete();

	UPROPERTY(EditAnywhere)
	class UGrowSpotData* GrowSpotData;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UMaterialInstanceDynamic* PlotMaterial{};

	void SpawnAPoof();

	UPROPERTY(Replicated, VisibleAnywhere)
	APickUpItem* ItemRef{};
	
	UPROPERTY(Replicated, EditAnywhere)
	EColours OwningPlayerColor{};

	UPROPERTY(Replicated, VisibleAnywhere)
	ABeehive* Beehive = nullptr;

protected:
	UFUNCTION()
	void OnRep_GrowSpotState();
	UPROPERTY(ReplicatedUsing=OnRep_GrowSpotState, VisibleAnywhere)
	EGrowSpotState GrowSpotState = EGrowSpotState::Empty;
	
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTime{10};
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTimer{};
	
	UFUNCTION()
	void OnRep_FertilisationState();
	UPROPERTY(ReplicatedUsing=OnRep_FertilisationState, EditAnywhere)
	FFertilisationState FertilisationState{};
	
	void ScalePlantOnTick() const;

	UFUNCTION()
	void MandrakePickupNoise();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_MandrakePickupNoise();
	
	void SetPlantReadySparkle(bool _bIsActive);
	
	//
	// Fertilisation
	//

	UPROPERTY(EditAnywhere)
	UMaterialInstance* TEMP_ConcreteMat;
	
	UPROPERTY(EditAnywhere)
	UMaterialInstance* GoldMaterial;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* HitBox;

	UPROPERTY(VisibleAnywhere)
	float ConcreteDamageTimer{};

	//
	//	SFX
	//

	UPROPERTY(EditAnywhere)
	USoundAttenuation* MandrakeAttenuationSettings;

	UPROPERTY(EditAnywhere)
	USoundCue* MandrakeScreamQueue;
	
	//
	// VFX
	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* PlantReadyComponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class TSubclassOf<AActor> PoofSystem;
	//
	// Prefabs
	//
	
	/* Planting from PlantData Data Asset */
	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<APlant> PlantPrefab;

	UPROPERTY(EditAnywhere, Category = Seeds, meta = (AllowPrivateAccess))
	TSubclassOf<ABeehive> BeehivePrefab;

	/* Planting from WeaponData Data Asset */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGrowableWeapon> WeaponPrefab;
	
};
