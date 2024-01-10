#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndGamePodium.generated.h"

UCLASS()
class PROTOTYPE2_API AEndGamePodium : public AActor
{
	GENERATED_BODY()
	
public:	
	class ACameraActor* GetEndGameCamera();
	class UArrowComponent* GetWinPosition(int32 _Placement);
	class UArrowComponent* GetLossPosition(int32 _Placement);
	
	UPROPERTY(EditAnywhere, Replicated)
	USceneComponent* BaseRootComponent;
	UPROPERTY(EditAnywhere, Replicated)
	TArray<class UStaticMeshComponent*> PodiumMeshes{nullptr, nullptr, nullptr};
	UPROPERTY(EditAnywhere, Replicated)
	TArray<class UArrowComponent*> WinPositions{nullptr, nullptr, nullptr, nullptr, nullptr};
	UPROPERTY(EditAnywhere, Replicated)
	TArray<class UArrowComponent*> LossPositions{nullptr, nullptr, nullptr, nullptr, nullptr};
	UPROPERTY(EditAnywhere, Replicated)
	class ACameraActor* EndGameCamera{nullptr};
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACameraActor> EndGameCameraPrefab;
protected:
	AEndGamePodium();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

protected:

	//UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	//TSubclassOf<class AEndGameCamera> EndGameCameraPrefab;
	

};
