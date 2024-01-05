#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Seed.generated.h"

class APlant;
UCLASS()
class PROTOTYPE2_API ASeed : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()


public:
	ASeed();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
public:

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleParachuteVisibility(bool _Visible);
	void Multi_ToggleParachuteVisibility_Implementation(bool _Visible);

	UPROPERTY(EditAnywhere)
	class UStaticMesh* WinterParachute;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* NormalParachute;
	
	UPROPERTY(EditAnywhere)
	float BobSpeed{120.0f};

	UPROPERTY(EditAnywhere)
	float BobAmplitude{8.0f};
	
	UPROPERTY(EditAnywhere)
	float DropDistance{400};

	UPROPERTY(EditAnywhere)
	float FallTime{3.0f};

	UPROPERTY(VisibleAnywhere)
	float SpawnTime;

	UFUNCTION()
	void HandleParachuteMovement();

	UPROPERTY(VisibleAnywhere)
	FVector SpawnPos;
	UPROPERTY(VisibleAnywhere)
	FRotator SpawnRotation;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ParachuteMesh;
	
	UPROPERTY(EditAnywhere)
	float GrowTime;

	UPROPERTY(EditAnywhere)
	bool bIsPlanted;

	UPROPERTY(EditAnywhere)
	bool bIsWeapon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlantToGrow;
};
