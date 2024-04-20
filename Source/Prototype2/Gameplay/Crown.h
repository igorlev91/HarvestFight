

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
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerTakesTheCrown, int32, _PlayerID);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerTakesTheCrown, FString, _PlayerName);
	UPROPERTY(BlueprintAssignable)
	FOnWinnerTakesTheCrown 	OnWinnerTakesTheCrownDelegate;

	UFUNCTION()
	void OnRep_AttachedPlayer();
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttachedPlayer)
	class APrototype2Character* AttachedPlayer;
	
private:
	ACrown();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	void AttachToCurrentWinner();
	
	UPROPERTY(VisibleAnywhere)
	class APrototype2Gamestate* GameStateRef;
	
	UPROPERTY(EditAnywhere)
	class USceneComponent* RootTransformComponent;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere)
	float UpdateDelay{3.0f};
	UPROPERTY(VisibleAnywhere)
	float UpdateDelayTimer{0.0f};
};
