
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoneyGoopPatch.generated.h"

UCLASS()
class PROTOTYPE2_API AHoneyGoopPatch : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHoneyGoopPatch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;
};
