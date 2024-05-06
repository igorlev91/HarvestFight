
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
	void Launch(APrototype2Character* _Player, bool _WithArrowDirection);
	
	UPROPERTY(EditAnywhere, Category="Strength")
	float ForwardStrength = 3000.0f;

	UPROPERTY(EditAnywhere, Category="Strength")
	float VerticalStrength = 3000.0f;

	float Delay = 0.2f;
	float Counter = 0.2f;

	UPROPERTY(EditAnywhere)
	bool bOnlyUp = false;

	UPROPERTY(EditAnywhere)
	bool bOnAndOff = false;
	UPROPERTY(EditAnywhere)
	float InitialDelay = 1.0f;
	UPROPERTY(EditAnywhere)
	float TimeOff = 1.0f;
	UPROPERTY(EditAnywhere)
	float TimeOn = 1.0f;

	UPROPERTY(EditAnywhere)
	float Timer = 1.0f;
	bool bTurnedOn = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UBoxComponent* LaunchCollider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMeshComponent* LaunchPadMesh;

	UPROPERTY(VisibleAnywhere)
	class USquashAndStretch* SquashAndStretch{nullptr};
};
