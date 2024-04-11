
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

class APlant;
class AGrowableWeapon;
class ASeed;
class AWeaponSeed;
UCLASS()
class PROTOTYPE2_API AGrowSpot : public AActor, public IInteractInterface
{
	friend class URandomEventManager;
	GENERATED_BODY()
	
public:	
	AGrowSpot();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float _DeltaTime) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	virtual bool IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete = true);
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID) override;

	// stealing
	virtual void HoldInteract(APrototype2Character* _Player) override;
	bool Stealing_IsInteractable_Unprotected(APrototype2Character* _Player);
	void Stealing_ClientInteract(APrototype2Character* _Player);
	void Stealing_Interact(APrototype2Character* _Player);
	void Stealing_OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID);
	
	void PlantASeed(ASeed* _SeedToPlant);
	void DestroyPlant();
	void CompleteGrowth();

	void DegradeConcrete();
	UFUNCTION(Server, Reliable)
	void Server_DegradeConcrete();

	UPROPERTY(EditAnywhere)
	class UGrowSpotData* GrowSpotData;
	
	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;
	UPROPERTY(VisibleAnywhere)
	FTimeline RiseTimeline{};
	UPROPERTY(VisibleAnywhere)
	float StartZHeight{};
	UPROPERTY(EditAnywhere)
	class UCurveFloat* RiseCurve{};
	//UPROPERTY(EditAnywhere)
	//class USquashAndStretch* SSComponent;

	UPROPERTY(VisibleAnywhere)
	ARadialPlot* RadialPlot;
	UPROPERTY(Replicated, VisibleAnywhere)
	AActor* GrowingActor{};
	UPROPERTY(Replicated, VisibleAnywhere)
	APickUpItem* GrowingItemRef{};
	
	UPROPERTY(Replicated, EditAnywhere)
	FString OwningPlayerName{};
	UPROPERTY(Replicated, EditAnywhere)
	EColours OwningPlayerColor{};

	UPROPERTY(Replicated, VisibleAnywhere)
	ABeehive* Beehive = nullptr;

	/* Variables for stealing */
	UPROPERTY()
	APrototype2Character* CurrentPlayerStealing = nullptr;
	float HoldInteractTotalDuration = 5.0f;
	float HoldInteractTimer = 0.0f;
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	EGrowSpotState GrowSpotState = EGrowSpotState::Empty;
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTime{10};
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTimer{};
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 ConcretedHealth{};
	UPROPERTY(EditAnywhere)
	int32 ConcreteTotalHealth{5};
	UPROPERTY(EditAnywhere)
	float ConcretedDamageInterval{0.2f};
	UPROPERTY(Replicated, VisibleAnywhere)
	float ConcretedDamageTimer{};

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bPoisoned{};
	
	UFUNCTION()
	void RiseTimelineUpdate(float _Delta);
	
	void GrowPlantOnTick(float _DeltaTime);
	void ScalePlantOnTick() const;

	void MandrakePickupNoise(APrototype2Character* _Player);

	void SetBeehive(ABeehive* _Beehive, float _GrowTime);
	
	void MakePlantGold();
	UFUNCTION(Server, Reliable)
	void Server_MakePlantGold();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_MakePlantGold();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_MakePlantConcrete();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_MakePlantPoison();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_BrakePlantConcrete();
	void SetPlantReadySparkle(bool _bIsActive);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetPlantReadySparkle(bool _bIsActive);
	
	//
	// Fertilisation
	//

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_UpdateMaterial();
	void UpdateMaterial();



	UPROPERTY(EditAnywhere)
	UMaterialInstance* DefaultMaterial;
	
	UPROPERTY(EditAnywhere)
	UMaterialInstance* GoldMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* PoisonMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* ConcreteMaterial;
	
	UPROPERTY(Replicated, EditAnywhere)
	bool bIsFertilised;

	UPROPERTY(EditAnywhere)
	float FertiliseInteractDelay = 0.1;

	UPROPERTY(Replicated, VisibleAnywhere)
	float FertiliseInteractDelayTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class UBoxComponent* HitBox{nullptr};

	bool bIsNormalGrowSpot = true;
	//
	//	SFX
	//

	UPROPERTY(EditAnywhere)
	USoundAttenuation* MandrakeAttenuationSettings;
	
	//
	// VFX
	//
	UPROPERTY(EditAnywhere, Category = VFX)
	class UNiagaraSystem* PlantReadySystem;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* PlantReadyComponent;

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
