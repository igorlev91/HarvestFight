
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "MovingPlatforms.generated.h"


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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UItemComponent* ItemComponent;

private:
	UPROPERTY(EditAnywhere)
	TArray<FVector> PositionArray;
	UPROPERTY(EditAnywhere)
	bool bMoveArrayBackwards;
	UPROPERTY(VisibleAnywhere)
	int32 PositionInMoveArray = 0;

	UPROPERTY(EditAnywhere)
	float TimeToMove;
	UPROPERTY(VisibleAnywhere)
	float TimeTrackerTimeToMove;
};
