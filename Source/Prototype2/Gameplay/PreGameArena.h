
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreGameArena.generated.h"

UCLASS()
class PROTOTYPE2_API APreGameArena : public AActor
{
	GENERATED_BODY()
	
public:
	APreGameArena();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* Mesh{nullptr};
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	class APlayerStart* PlayerStart{nullptr};

	UPROPERTY()
	class APrototype2Gamestate* GameState;
};
