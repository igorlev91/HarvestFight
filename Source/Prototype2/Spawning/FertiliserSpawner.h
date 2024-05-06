
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "FertiliserSpawner.generated.h"

class AFertiliser;
UCLASS()
class PROTOTYPE2_API AFertiliserSpawner : public AActor, public IInteractInterface
{
	GENERATED_BODY()

	/* Public Functions */
public:	
	AFertiliserSpawner();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	/* Interaction functions */
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode = INVALID) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnClientWalkAway(APrototype2Character* _Player) override;

	UFUNCTION()
	bool HasSpawnedFruit();

	UFUNCTION()
	void UpdateFertiliserMaterial();
	/* Public Variables */
public:	
	class UStaticMeshComponent* GetMesh();

		
	UPROPERTY(EditAnywhere)
	TArray<class USeedData*> FertiliserDatas;

	UPROPERTY(Replicated, VisibleAnywhere)
	class AFertiliser* SpawnedFertiliser{nullptr};

	/* Protected Functions */
protected:
	void GenerateFertiliserOnTimer(float _DeltaTime);
	void SpawnFertiliser();
	
	void CheckForTooManyFertiliserBags();
protected:
	
	UPROPERTY(EditAnywhere)
	class UMaterialInstance* GhostMaterial{};
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ChickenMesh{};
	
	UPROPERTY(EditAnywhere)
	int32 ChanceOfConcrete_Inverse{3};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Gamestate* GameStateRef;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class AFertiliser> FertiliserPrefab;
	
	UPROPERTY(VisibleAnywhere)
	bool bBagCountReachedMax{false};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SpawnInterval{10.0f};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	float SpawnTimer{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SpawnZPosition{100.0f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SpawnXPosition{100.0f};
};
