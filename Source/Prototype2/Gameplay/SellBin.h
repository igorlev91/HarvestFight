#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Interface/CullingInterface.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "SellBin.generated.h"

class UWidgetComponent;
class APrototype2Character;
class APlant;

USTRUCT()
struct FServerSellData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 SellValue{};
	
	UPROPERTY(VisibleAnywhere)
	int32 StarValue{};

	UPROPERTY(VisibleAnywhere)
	class APrototype2PlayerState* LastPlayerStateToSell{nullptr};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Character* LastPlayerToSell{nullptr};

	UPROPERTY(VisibleAnywhere)
	float TimeOfSell{};
};

UCLASS()
class PROTOTYPE2_API ASellBin : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* , EInteractMode _ForcedMode = INVALID) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSold, int32, _PlayerID, int32, _Score);
	UPROPERTY(BlueprintAssignable)
	FOnItemSold OnItemSoldDelegate;

	UPROPERTY(EditAnywhere)
	class USellBinData* SellBinData;
	
protected:
	ASellBin();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	UFUNCTION()
	void OnPlayerTouchSellBin(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void SellOnThrown(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class TSubclassOf<AActor> PoofSystem;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UNiagaraComponent* InteractSystem;

	UFUNCTION()
	void OnRep_ItemSold();
	
	UPROPERTY(ReplicatedUsing=OnRep_ItemSold, VisibleAnywhere)
	FServerSellData ServerSellData{};

	UPROPERTY(EditAnywhere)
	UBoxComponent* ThrowToSellCollider;

	UPROPERTY(EditAnywhere)
	class USoundCue* SellCue{nullptr};

	UPROPERTY(EditAnywhere)
	class USoundCue* EnemySellCue{nullptr};
};
