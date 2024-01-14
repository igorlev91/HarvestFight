
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CullingInterface.generated.h"

UINTERFACE(NotBlueprintable)
class UCullingInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOTYPE2_API ICullingInterface
{
	GENERATED_BODY()
	
	/* Public Functions */
public:
	// Function to handle visibility based on camera proximity
	UFUNCTION(BlueprintCallable)
	virtual void HandleVisibilityActor(APlayerController* _PlayerController, AActor* _Actor);
	
	UFUNCTION(BlueprintCallable)
	virtual void HandleVisibilityStaticMesh(APlayerController* _PlayerController, AActor* _Actor, UStaticMeshComponent* _Mesh);
	
	/* protected Variables */
protected:
	float CullingDistance = 500.0f; // Distance from camera
	
	bool bIsCulledByCamera = true; // If this object should be culled
	
};
