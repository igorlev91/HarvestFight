#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prototype2/Widgets/Widget_PlayerHUD.h"
#include "ItemComponent.generated.h"

class APrototype2Character;
class APickUpItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE2_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UItemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	
public:
	void Interact(APrototype2Character* _Player, APickUpItem* _ItemPickedUp);
	
	/* Spawning from PlantData Data Asset */
	void InitializeSeed(TArray<UMaterialInstance*> _InMaterials, UStaticMesh* _InMesh);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DisableCollisionAndAttach();

	UFUNCTION()
	void SetStencilEnabled(bool _StencilEnabled);
	
public:
	/* Delegate to tell animation blueprint when the player is at endgame podium */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoldChanged);
	UPROPERTY()
	FOnGoldChanged OnGoldChangedDelegate;
	
	UPROPERTY(Replicated, EditAnywhere)
	bool bDoBeginPlay = true;;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnRep_bGold();
	
	UPROPERTY(ReplicatedUsing = OnRep_bGold, EditAnywhere)
	bool bGold{false};

	UPROPERTY(VisibleAnywhere)
	bool bInitialized{};

	// For throwing item into bin 
	UPROPERTY(Replicated)
	APrototype2Character* PlayerWhoThrewItem;
};
