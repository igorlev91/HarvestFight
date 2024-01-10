
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

class APrototype2Character;
class UBoxComponent;
UCLASS()
class PROTOTYPE2_API ALaunchPad : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaunchPad();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LaunchPlayer(APrototype2Character* _Player);

	UPROPERTY(EditDefaultsOnly)
	float ForwardStrength = 3000.0f;

	UPROPERTY(EditDefaultsOnly)
	float VerticalStrength = 3000.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UBoxComponent* LaunchCollider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMeshComponent* LaunchPadMesh;
};
