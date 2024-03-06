
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/Gameplay/MovingPlatforms.h"
#include "AspearagusProjectile.generated.h"

class APrototype2Character;
UCLASS()
class PROTOTYPE2_API AAspearagusProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAspearagusProjectile();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckForHitPlayers();
	bool CheckForHitObstacle();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_InitializeProjectile(APrototype2Character* _Player, UStaticMesh* _Mesh, float _Speed, float _LifeTime, float _AttackSphereRadius, float _AttackChargeAmount);
	
	UPROPERTY(Replicated, VisibleAnywhere)
	float AttackSphereRadius;

	UPROPERTY(Replicated, VisibleAnywhere)
	APrototype2Character* OwningPlayer;

	UPROPERTY(Replicated, VisibleAnywhere)
	UStaticMeshComponent* AspearagusMesh;

	UPROPERTY(Replicated, VisibleAnywhere)
	float Speed = 50.0f;
	UPROPERTY(VisibleAnywhere)
	float DeathTimer = 10.0f;
	UPROPERTY(Replicated, VisibleAnywhere)
	float ChargeAmount = 0;

	UPROPERTY(VisibleAnywhere)
	bool bHitSomething{};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector PredictFuturePosition(float PredictionTime);
	bool FindClosestTimestamps(float TargetTime, int32& Index1, int32& Index2);

	void DoMovementLogic(float DeltaTime);

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<FVector> PreviousPositions;
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<float> Timestamps;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(Replicated, VisibleAnywhere)
	FVector StartPosition{-1337.0f, -1337.0f, -1337.0f};
	
	UPROPERTY(Replicated, VisibleAnywhere)
	FQuat StartRotation{FQuat::Identity};

	UPROPERTY(EditAnywhere)
	bool bDoRotation{false};
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlatformPosition)
	FVector ReplicatedPlatformPosition;

	UPROPERTY(ReplicatedUsing = OnRep_PlatformPosition)
	FRotator ReplicatedPlatformRotation;

	UFUNCTION()
	void OnRep_PlatformPosition();

	
};
