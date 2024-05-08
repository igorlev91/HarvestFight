
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
class APrototype2PlayerState;

USTRUCT()
struct FServerGrowspotDetails
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	APrototype2PlayerState* LastPlayerToInteract{nullptr};

	UPROPERTY(VisibleAnywhere)
	int32 CropStarValue{};
	
	UPROPERTY(VisibleAnywhere)
	float LastTimeOfInteract{};
};

UCLASS()
class PROTOTYPE2_API AGrowSpot : public AActor, public IInteractInterface
{
	friend class ABeehive;
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
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode = INVALID) override;
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

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DamageConcrete();

	UFUNCTION()
	void UpdateGrowUI();

	UFUNCTION()
	void UpdateGrowUIVisibility();

	UFUNCTION()
	bool CheckForSelfConcreting(APrototype2PlayerState* _Player);

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* GrowWidgetComponent;

	UPROPERTY(VisibleAnywhere)
	class UWidget_3DGrowUI* GrowWidget;

	UPROPERTY(EditAnywhere)
	class UGrowSpotData* GrowSpotData;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UMaterialInstanceDynamic* PlotMaterial{};

	void SpawnAPoof();

	UFUNCTION()
	void OnRep_ItemRef();
	
	UPROPERTY(ReplicatedUsing=OnRep_ItemRef, VisibleAnywhere)
	APickUpItem* ItemRef{};

	UPROPERTY(VisibleAnywhere)
	class APickUpItem* LastItemRef{nullptr};
	
	UPROPERTY(Replicated, EditAnywhere)
	EColours OwningPlayerColor{};

	UPROPERTY(Replicated, VisibleAnywhere)
	ABeehive* Beehive = nullptr;

	UFUNCTION()
	void OnRep_FertilisationState();
	UPROPERTY(ReplicatedUsing=OnRep_FertilisationState, EditAnywhere)
	FFertilisationState FertilisationState{};
	
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
	void OnRep_ServerGrowspotDetails();

	UPROPERTY(ReplicatedUsing=OnRep_ServerGrowspotDetails, VisibleAnywhere)
	FServerGrowspotDetails ServerGrowspotDetails{};
	
	void ScalePlantOnTick() const;

	/* IF this plant is the highest value on this map, it will have a sound in the data asset to play */
	UFUNCTION()
	void HighValuePickupNoise();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_HighValuePickupNoise();
	
	void SetPlantReadySparkle(bool _bIsActive, bool _IsBeehive = false);
	
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

	UPROPERTY(VisibleAnywhere)
	class USoundCue* PlantCue{nullptr};

	UPROPERTY(VisibleAnywhere)
	class USoundAttenuation* PlantAttenuation{nullptr};
	
	UPROPERTY(EditAnywhere)
	USoundAttenuation* HighValueAttenuationSettings;
	
	//
	// VFX
	//

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSold, int32, _PlayerID, int32, _Score);
	UPROPERTY(BlueprintAssignable)
	FOnItemSold OnItemSoldDelegate;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* PlantReadyComponent;
	UPROPERTY(EditAnywhere, Category = VFX)
	class UNiagaraSystem* PlantReadySystem{nullptr};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class TSubclassOf<AActor> PoofSystem;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* ConcreteBreakComponent;

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

	UPROPERTY(VisibleAnywhere)
	class APrototype2Character* LastLocalPlayer{nullptr};

	UPROPERTY(EditAnywhere)
	float UIVisiblityRadius{1000.0f};
};
