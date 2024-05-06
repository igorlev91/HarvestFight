
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

USTRUCT()
struct FMPMove
{
	GENERATED_BODY()

	FMPMove() {}

	UPROPERTY()
	float movementSpeed = 0.0f;
	UPROPERTY()
	float movementAmplitude = 0.0f;
	UPROPERTY()
	float deltaTime = 0.0f;
	UPROPERTY()
	float time = 0.0f;
};

USTRUCT()
struct FMPServerState
{
	GENERATED_BODY()

	FMPServerState() {}

	UPROPERTY()
	FMPMove currentMove{};
	UPROPERTY()
	FTransform transform = FTransform::Identity;
};

#define MAX_POSITION_HISTORY 4

UCLASS()
class PROTOTYPE2_API AMovingPlatforms : public AActor
{
	GENERATED_BODY()
public:	
	AMovingPlatforms();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	FVector PredictFuturePosition(float PredictionTime);
	bool FindClosestTimestamps(float TargetTime, int32& Index1, int32& Index2);

	void DoMovementLogic(float DeltaTime);

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<FVector> PreviousPositions;
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<FQuat> PreviousRotations;
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<float> Timestamps;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(Replicated, VisibleAnywhere)
	FVector StartPosition{-1337.0f, -1337.0f, -1337.0f};
	
	UPROPERTY(Replicated, VisibleAnywhere)
	FQuat StartRotation{FQuat::Identity};

	UPROPERTY(EditAnywhere)
	bool bDoRotation{false};
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlatformPosition)
	FVector ReplicatedPlatformPosition;

	UPROPERTY(ReplicatedUsing = OnRep_PlatformPosition)
	FRotator ReplicatedPlatformRotation;

	UFUNCTION()
	void OnRep_PlatformPosition();
public:	
	// Called every frame

	void MovePlatform(float DeltaTime);

	void MovePlatformForward();
	void MovePlatformBackwards();
	void MovePlatformLoop();

	void Rotate(float DeltaTime);
	void SineWave(float DeltaTime);
	



	//UFUNCTION(Server, Reliable)
	//void ServerMovePlatform(FVector NewLocation, FRotator NewRotation);

private:
	//UPROPERTY(Replicated)
	//double ServerStartTime;

	
	UPROPERTY(EditAnywhere)
	bool bUseBaseTimers = true;
	
	// movement
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bDoMove;
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bDoLoop;
	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<FMovementInfo> MovementInfoArray;
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bMoveArrayBackwards;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	int32 PositionInMoveArray = 0;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TimeToMove;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float BaseTimeStop;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float TimeTrackerTimeToMove;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float TimeStop;

	UPROPERTY(EditAnywhere, Category = "Movement")
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
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float SineTime;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float SineStartTimeMin;
	UPROPERTY(EditAnywhere, Category = "Sine Wave")
	float SineStartTimeMax;
//
	//bool bPredictMovement;
};
