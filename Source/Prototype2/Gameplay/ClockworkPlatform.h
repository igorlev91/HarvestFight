

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClockworkPlatform.generated.h"

UCLASS()
class PROTOTYPE2_API AClockworkPlatform : public AActor
{
	GENERATED_BODY()
protected:
	AClockworkPlatform();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void TickTimeline(float _Progress);
	UFUNCTION()
	void OnTimelineEnd();
	UFUNCTION()
	void OnHaltEnd();
protected:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Platform;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Pole;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* EndPoint_DEBUG;

	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* LerpTimeline;

	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* HaltTimeline;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector StartPosition{};

	UPROPERTY(VisibleAnywhere)
	float LerpProgress{};

	UPROPERTY(VisibleAnywhere)
	bool OnReturnJourney{};

	UPROPERTY(VisibleAnywhere)
	float PauseTimer{};
	
protected:
	UPROPERTY(EditAnywhere, Category="Moving Platform")
	float MoveSpeed{0.3183f};

	UPROPERTY(EditAnywhere, Category="Moving Platform")
	float PauseDuration{5.0f};
};
