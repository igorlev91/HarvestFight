
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

	/* For networking */
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckForHitPlayers();
	bool CheckForHitObstacle();
	void InitializeProjectile(APrototype2Character* _Player, UStaticMesh* _Mesh, float _Speed, float _LifeTime, float _AttackSphereRadius);
	
	float AttackSphereRadius;

	APrototype2Character* OwningPlayer;

	//UPROPERTY(Replicated)
	UStaticMeshComponent* AspearagusMesh;

	float Speed = 100.0f;
	float DeathTimer = 10.0f;
	float ChargeAmount = 0;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
