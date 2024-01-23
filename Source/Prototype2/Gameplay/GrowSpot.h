
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "NiagaraComponent.h"
#include "RadialPlot.h"
#include "Components/TimelineComponent.h"
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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float _DeltaTime) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID) override;

	void PlantASeed(ASeed* _SeedToPlant);
	void DestroyPlant();
	
	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;
	UPROPERTY(VisibleAnywhere)
	FTimeline RiseTimeline{};
	UPROPERTY(VisibleAnywhere)
	float StartZHeight{};
	UPROPERTY(EditAnywhere)
	class UCurveFloat* RiseCurve{};
	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;

	UPROPERTY(VisibleAnywhere)
	ARadialPlot* RadialPlot;
	UPROPERTY(Replicated, VisibleAnywhere)
	AActor* GrowingActor{};
	UPROPERTY(Replicated, VisibleAnywhere)
	APickUpItem* GrowingItemRef{};
	
	UPROPERTY(Replicated, EditAnywhere)
	FString OwningPlayerName{};

	UPROPERTY(Replicated, VisibleAnywhere)
	ABeehive* Beehive = nullptr;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	EGrowSpotState GrowSpotState = EGrowSpotState::Empty;
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTime{10};
	UPROPERTY(Replicated, EditAnywhere)
	float GrowTimer{};

	UFUNCTION()
	void RiseTimelineUpdate(float _Delta);
	
	void GrowPlantOnTick(float _DeltaTime);
	void ScalePlantOnTick() const;

	void MandrakePickupNoise(APrototype2Character* _Player);

	void SetBeehive(ABeehive* _Beehive, float _GrowTime);
	void MakePlantGold();

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_MakePlantGold();
	void SetPlantReadySparkle(bool _bIsActive);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetPlantReadySparkle(bool _bIsActive);
	
	//
	// Fertilisation
	//

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_UpdateMaterial();
	void UpdateMaterial();

	UPROPERTY(Replicated, EditAnywhere)
	UMaterialInstance* DefaultMaterial;
	
	UPROPERTY(Replicated, EditAnywhere)
	UMaterialInstance* GoldMaterial;
	
	UPROPERTY(Replicated, EditAnywhere)
	bool bIsFertilised;

	UPROPERTY(EditAnywhere)
	float FertiliseInteractDelay = 0.1;

	UPROPERTY(Replicated, VisibleAnywhere)
	float FertiliseInteractDelayTimer;

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
