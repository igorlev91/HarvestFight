

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
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerTakesTheCrown, int32, _PlayerID);
	UPROPERTY(BlueprintAssignable)
	FOnWinnerTakesTheCrown 	OnWinnerTakesTheCrownDelegate;
;
private:
	ACrown();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	void AttachToCurrentWinner();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_AttachToCurrentWinner(USkeletalMeshComponent* _WinterMesh);

	UPROPERTY(VisibleAnywhere)
	class APrototype2Gamestate* GameStateRef;
	
	UPROPERTY(EditAnywhere, Replicated)
	class USceneComponent* RootTransformComponent;
	
	UPROPERTY(EditAnywhere, Replicated)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UBobTransformComponent* BobTransformComponent;

	UPROPERTY(VisibleAnywhere)
	float UpdateDelay{3.0f};
	UPROPERTY(VisibleAnywhere)
	float UpdateDelayTimer{0.0f};

	UFUNCTION(Server, Reliable)
	void Server_OnWinnerTakesTheCrown(int32 _PlayerID);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnWinnerTakesTheCrown(int32 _PlayerID);
};
