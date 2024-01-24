

#pragma once

#include "CoreMinimal.h"
#include "IndexTypes.h"
#include "GameFramework/Actor.h"
#include "Prototype2/Gameplay/RadialPlot.h"
#include "RaidialSpawner.generated.h"

UCLASS()
class PROTOTYPE2_API ARaidialSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	/* Sets default values for this actor's properties */
	ARaidialSpawner();

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere)
	bool bShouldChangePlayerCount = true;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ARadialPlot> PlotPrefab;

	UPROPERTY(EditAnywhere)
	USceneComponent* InitialSpawn;

	UPROPERTY(EditAnywhere)
	int32 PlayerCount = 12;

	/* Sets up the radial plot with correct positioning and rotations based on the number of players */
	void SetUp();

	UPROPERTY()
	TMap<EColours, ARadialPlot*> Plots;
	
	UFUNCTION(BlueprintCallable)
	void SetupDelayed();
};
