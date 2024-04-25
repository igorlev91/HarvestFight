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
UCLASS()
class PROTOTYPE2_API ASellBin : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSold, int32, _PlayerID);
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
	
	UFUNCTION(Server, Reliable)
	void Server_FireParticleSystem();
	void Server_FireParticleSystem_Implementation();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireSellVFX(APrototype2Character* _Player, int32 _SellAmount = 30);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireSellVFXCrop(APrototype2Character* _Player, class APlant* _Plant);
	
	UFUNCTION(Client, Reliable)
	void Client_MoveUI(float _DeltaTime);
	void Client_MoveUI_Implementation(float _DeltaTime);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem();
	
	void HideParticleSystem();
	void MoveUIComponent(float _Dt);
	
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerSell(APrototype2Character* _Player);
	UFUNCTION(Server, Reliable)
	void Server_OnPlayerSell(APrototype2Character* _Player, APlant* _Plant);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnPlayerSell(APrototype2Character* _Player);
	UFUNCTION(Client, Reliable)
	void Client_Boing();

	UFUNCTION()
	void SellOnThrown(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
protected:
	UPROPERTY(EditAnywhere)
	int32 LoosingPlayerCanAddMatchTimeTime{20};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UItemComponent* ItemComponent;
	
	// Selling UI
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UWidgetComponent* SellAmountWidgetComponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector StartPosition{};
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	bool bIsMoving{};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float MoveSpeed{};
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float MovingTime{};
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	float MovingTimer{};
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	bool bWidgetVisible{};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UNiagaraSystem* ParticleSystem;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class TSubclassOf<AActor> PoofSystem;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UNiagaraComponent* InteractSystem;

	//UPROPERTY(EditAnywhere)
	//class USquashAndStretch* SSComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ThrowToSellCollider;
};
