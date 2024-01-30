#pragma once

#include "CoreMinimal.h"
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
	virtual void HoldInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
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
	void FireSellFX(APlant* _InPlant, APrototype2Character* _Player);
	UFUNCTION(Client, Reliable)
	void Client_MoveUI(float _DeltaTime);
	void Client_MoveUI_Implementation(float _DeltaTime);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem();
	void Multi_FireParticleSystem_Implementation();
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
	class UNiagaraComponent* InteractSystem;

	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;
};
