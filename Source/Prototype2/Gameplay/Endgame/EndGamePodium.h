#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndGamePodium.generated.h"

UCLASS()
class PROTOTYPE2_API AEndGamePodium : public AActor
{
	GENERATED_BODY()
	
public:	
	class AEndGameCamera* GetEndGameCamera();
	class UArrowComponent* GetWinPosition(int32 _Placement);
	class UArrowComponent* GetLossPosition(int32 _Placement);

protected:
	AEndGamePodium();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P1LosePosition;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P2LosePosition;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P3LosePosition;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P4LosePosition;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P1WinPosition;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P2WinPosition;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P3WinPosition;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UArrowComponent* P4WinPosition;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<class UArrowComponent*> WinPositions;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<class UArrowComponent*> LossPositions;
	
	UPROPERTY(Transient, Replicated, EditAnywhere, meta = (AllowPrivateAccess))
	class AEndGameCamera* EndGameCamera;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class AEndGameCamera> EndGameCameraPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<class UStaticMeshComponent*> PodiumMeshes;
};
