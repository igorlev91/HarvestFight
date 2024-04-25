
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetArenaColour();
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* Mesh{nullptr};

	UPROPERTY(VisibleAnywhere)
	class UMaterialInstanceDynamic* TeamsMaterial{};
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 AssignedTeam{};
protected:
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	class APlayerStart* PlayerStart{nullptr};

	UPROPERTY()
	class APrototype2Gamestate* GameState;

	UPROPERTY(EditAnywhere)
	class UColourData* ColourData;
};
