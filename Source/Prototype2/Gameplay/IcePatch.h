
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IcePatch.generated.h"

UCLASS()
class PROTOTYPE2_API AIcePatch : public AActor
{
	GENERATED_BODY()
	
public:	
	AIcePatch();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;
};
