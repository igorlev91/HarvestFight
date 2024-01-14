
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/Pickups/ItemComponent.h"
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

	void MovePlatform();

	void MovePlatformForward();
	void MovePlatformBackwards();

	void Rotate(float DeltaTime);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UItemComponent* ItemComponent;

private:

	UPROPERTY(EditAnywhere)
	bool bUseBaseTimers = true;
	
	// movement
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bDoMove;
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

	//rotation
	UPROPERTY(EditAnywhere, Category = "Rotation")
	bool bDoRotate;
	UPROPERTY(EditAnywhere, Category = "Rotation")
	int32 RotationSpeed;
};
