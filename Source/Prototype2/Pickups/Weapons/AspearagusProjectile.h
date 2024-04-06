﻿
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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_InitializeProjectile(APrototype2Character* _Player, UStaticMesh* _Mesh, float _Speed, float _LifeTime, float _AttackSphereRadius, float _AttackChargeAmount);
	
	UPROPERTY(Replicated, VisibleAnywhere)
	float AttackSphereRadius;

	UFUNCTION()
	void OnRep_Initialization();
	
	UPROPERTY(ReplicatedUsing=OnRep_Initialization, VisibleAnywhere)
	APrototype2Character* OwningPlayer;

	UPROPERTY(Replicated, VisibleAnywhere)
	UStaticMeshComponent* AspearagusMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere)
	float DeathTimer = 10.0f;
	UPROPERTY(Replicated, VisibleAnywhere)
	float ChargeAmount = 0;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereCollision;

	
};
