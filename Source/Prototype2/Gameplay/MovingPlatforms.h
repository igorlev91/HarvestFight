
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatforms.generated.h"

USTRUCT()
struct FMovementInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FVector Position = {0.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere)
	float TimeToStopFor = 0.f;

	UPROPERTY(EditAnywhere)
	float TimeToMove = 0.f;
};

UCLASS()
class PROTOTYPE2_API AMovingPlatforms : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatforms();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void MovePlatform();

	void MovePlatformForward();
	void MovePlatformBackwards();

	void Rotate(float DeltaTime);

	void SineWave(float DeltaTime);

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION(Server, Reliable)
	void ServerMovePlatform(FVector NewLocation, FRotator NewRotation);

private:

	UPROPERTY(EditAnywhere)
	bool bUseBaseTimers = true;
	
	// movement
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bDoMove;
	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<FMovementInfo> MovementInfoArray;
	UPROPERTY(Replicated, EditAnywhere, Category = "Movement")
	bool bMoveArrayBackwards;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Movement")
	int32 PositionInMoveArray = 0;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TimeToMove;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float BaseTimeStop;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Movement")
	float TimeTrackerTimeToMove;
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Movement")
	float TimeStop;

	UPROPERTY(Replicated, EditAnywhere, Category = "Movement")
	float InitialDelay = 0;

	//rotation
	UPROPERTY(EditAnywhere, Category = "Rotation")
	bool bDoRotate;
	UPROPERTY(EditAnywhere, Category = "Rotation")
	int32 RotationSpeed;

	// sine wave
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	bool bDoSineWave;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float Amplitude;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float Frequency;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	FVector SineHeightBounce;
	UPROPERTY(Replicated, EditAnywhere, Category = "Sine Wave")
	float SineTime;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float SineStartTimeMin;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float SineStartTimeMax;

	bool bPredictMovement;
};
