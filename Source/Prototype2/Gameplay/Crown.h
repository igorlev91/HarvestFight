
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crown.generated.h"

class UBobTransformComponent;
UCLASS()
class PROTOTYPE2_API ACrown : public AActor
{
	GENERATED_BODY()

public:
	class UStaticMeshComponent* GetMesh();
	
private:
	ACrown();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Replicated)
	class USceneComponent* RootTransformComponent;
	
	UPROPERTY(EditAnywhere, Replicated)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UBobTransformComponent* BobTransformComponent;
};
