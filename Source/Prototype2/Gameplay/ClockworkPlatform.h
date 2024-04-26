

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClockworkPlatform.generated.h"

UCLASS()
class PROTOTYPE2_API AClockworkPlatform : public AActor
{
	GENERATED_BODY()
protected:
	AClockworkPlatform();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void DoMovement();

	UFUNCTION()
	float TriangleWave(float _X);
	
protected:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* StartPosition;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* EndPosition;

	UFUNCTION()
	void OnRep_ServerTime();
	
	UPROPERTY(ReplicatedUsing=OnRep_ServerTime, VisibleAnywhere)
	float ServerTime{};

	UPROPERTY(Replicated)
	float PlatformTimeOffset{0};

	UPROPERTY(VisibleAnywhere)
	float ClientTimeOffset{0};

	UPROPERTY(VisibleAnywhere)
	float ObservingPlayerPing{0};

	UPROPERTY(VisibleAnywhere)
	class ULocalPlayer* LocalPlayer{nullptr};
	UPROPERTY(VisibleAnywhere)
	class APlayerController* LocalPlayerController{nullptr};
	UPROPERTY(VisibleAnywhere)
	class APlayerState* LocalPlayerState{nullptr};
	
protected:
	UPROPERTY(EditAnywhere, Category="Moving Platform")
	float MoveSpeed{1};

	UPROPERTY(EditAnywhere, Category="Moving Platform")
	class UCurveFloat* MovementCurve;
};
