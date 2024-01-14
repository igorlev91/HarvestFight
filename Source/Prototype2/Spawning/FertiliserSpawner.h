
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
	
public:	
	AFertiliserSpawner();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
	class UStaticMeshComponent* GetMesh();
	
public:	
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;

protected:
	void GenerateFertiliserOnTimer(float _DeltaTime);
	void SpawnFertiliser();

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class AFertiliser> FertiliserPrefab;

	UPROPERTY(Replicated, VisibleAnywhere, meta = (AllowPrivateAccess))
	class AFertiliser* SpawnedFertiliser{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SpawnInterval{10.0f};

	UPROPERTY(Replicated, VisibleAnywhere, meta = (AllowPrivateAccess))
	float SpawnTimer{};
};
