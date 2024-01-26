
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UFUNCTION()
	void InitializeProjectile(APrototype2Character* _Player, UStaticMesh* _Mesh, float _Speed, float _LifeTime, float _AttackSphereRadius);
	
	UPROPERTY(VisibleAnywhere)
	float AttackSphereRadius;

	UPROPERTY(VisibleAnywhere)
	APrototype2Character* OwningPlayer;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AspearagusMesh;

	UPROPERTY(VisibleAnywhere)
	float Speed = 100.0f;
	UPROPERTY(VisibleAnywhere)
	float DeathTimer = 10.0f;
	UPROPERTY(VisibleAnywhere)
	float ChargeAmount = 0;

	UPROPERTY(VisibleAnywhere)
	bool bHitSomething{};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
