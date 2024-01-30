   

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallOffRespawn.generated.h"

UCLASS()
class PROTOTYPE2_API AFallOffRespawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFallOffRespawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RemovePhysicsObjectsBelowHeight();
	void RespawnPlayersBelowHeight();
	void CheckTimer(float DeltaTime);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	int32 RespawnHeight = -10000;
	inline constexpr static float TimeBetweenChecks{5.0f};
	float TimeCounter{};

	UPROPERTY(EditAnywhere)
	FVector RespawnLocation;
};
