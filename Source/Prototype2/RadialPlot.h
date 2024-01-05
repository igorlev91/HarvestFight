// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RadialPlot.generated.h"

UCLASS()
class PROTOTYPE2_API ARadialPlot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadialPlot();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<class AGrowSpot*> growSpots;
	UPROPERTY(EditAnywhere)
	int Player_ID;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlotMaterial(int _id);
	void Multi_SetPlotMaterial_Implementation(int _id);

	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlotSignMaterials;

	UPROPERTY(Replicated, EditAnywhere)
	class UStaticMeshComponent* PlotSignMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGrowSpot> GrowSpotPrefab;

	UPROPERTY(EditAnywhere)
	float PlotSpread {180.0f};

	UPROPERTY(EditAnywhere)
	float PlotZHeight {-20.0f};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void SetPlayerID(int _id);
};
