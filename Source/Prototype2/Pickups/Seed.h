

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
	virtual void Destroyed() override;

	UFUNCTION()
	void HandleParachuteMovement();
	
public:
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode = INVALID) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
public:
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
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ParachuteMesh;

	UFUNCTION()
	void OnRep_ParachuteMesh();
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_ParachuteMesh)
	class UStaticMesh* ParachuteSM{nullptr};

	UFUNCTION()
	void OnRep_bHasLanded();
	UPROPERTY(ReplicatedUsing=OnRep_bHasLanded)
	bool bHasLanded;

	// Wilting
public:
	UFUNCTION()
	void Wilt(float DeltaTime);

	virtual void Server_Drop() override;
	
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
