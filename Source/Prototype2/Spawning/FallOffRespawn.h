   

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallOffRespawn.generated.h"

class APrototype2Character;

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
	void Multi_RemovePhysicsObjectsBelowHeight();
	void Multi_RespawnPlayersBelowHeight();
	void CheckTimer(float DeltaTime);
	UFUNCTION(Client, Reliable)
	void Client_ClearItem(APrototype2Character* _Player);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	int32 RespawnHeight = -10000;
	inline constexpr static float TimeBetweenChecks{5.0f};
	float TimeCounter{};

	UPROPERTY(EditAnywhere)
	FVector RespawnLocation;
};
