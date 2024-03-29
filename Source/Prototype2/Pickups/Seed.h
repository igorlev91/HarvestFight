

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

	UFUNCTION()
	void HandleParachuteMovement();
public:
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void HoldInteract(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
public:

	UFUNCTION(Server, Reliable)
	void Server_SetParachuteMesh(UStaticMesh* _InMesh);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetParachuteMesh(UStaticMesh* _InMesh);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleParachuteVisibility(bool _Visible);
	void Multi_ToggleParachuteVisibility_Implementation(bool _Visible);

	void SetParachuteMesh(UStaticMesh* _InMesh);
	
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

	UPROPERTY(VisibleAnywhere)
	FVector SpawnPos;
	UPROPERTY(VisibleAnywhere)
	FRotator SpawnRotation;
	
	UPROPERTY(Replicated, EditAnywhere)
	class UStaticMeshComponent* ParachuteMesh;
	
	UPROPERTY(EditAnywhere)
	float GrowTime;

	UPROPERTY(EditAnywhere)
	bool bIsPlanted;

	UPROPERTY(EditAnywhere)
	bool bIsWeapon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlantToGrow;

	UPROPERTY(Replicated)
	bool bIsParachuteStaticMeshSet;

	UPROPERTY(Replicated)
	bool bHasLanded;

	// Wilting
public:
	UFUNCTION()
	void Wilt(float DeltaTime);
	UFUNCTION()
	void WiltMaterial();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnInteract();
	virtual void Server_Drop() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnDestroy();
	
	UPROPERTY(VisibleAnywhere)
	TArray<class UMaterialInstanceDynamic*> Materials;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> DestroyVFX{};

	UPROPERTY(EditAnywhere)
	float InitialLifetime{30};
	UPROPERTY(VisibleAnywhere)
	float Lifetime{};
	
	UPROPERTY(EditAnywhere)
	float WiltDelay{30};
	UPROPERTY(VisibleAnywhere)
	float WiltDelayTimer{};

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bShouldWilt{};
};
