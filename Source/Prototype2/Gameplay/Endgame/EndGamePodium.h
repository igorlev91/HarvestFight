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

	void PlayConfetteVFX();

	UFUNCTION(Server, Reliable)
	void Server_PlayConfetteVFX();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayConfetteVFX();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Podium Camera")
	FVector CameraPosition{560.0f,90.0f,170.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Podium Camera")
	FRotator CameraRotation{5.63f,185.6f,0};
	
protected:
	AEndGamePodium();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* WinConfetteComponent;
	
	//UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	//TSubclassOf<class AEndGameCamera> EndGameCameraPrefab;
	

};
