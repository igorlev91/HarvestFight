

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "SkyAlter.generated.h"

UCLASS()
class PROTOTYPE2_API ASkyAlter : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkyAlter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float _DeltaTime) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player) override;
	virtual bool IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete = true);
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID) override;

	UFUNCTION(Client, Reliable)
	void Client_ClearItem(APrototype2Character* _Player);

	UFUNCTION()
	void OnPlayerTouchAltar(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	UPROPERTY(EditAnywhere)
	UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent3;
};
