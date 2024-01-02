// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SellBin.h"
#include "SellBin_Winter.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UBoxComponent;

UCLASS()
class PROTOTYPE2_API ASellBin_Winter : public ASellBin
{
	GENERATED_BODY()

	ASellBin_Winter();

public:
	virtual void BeginPlay() override;
	
	void GetHit(float AttackCharge, float _maxAttackCharge, FVector AttackerLocation);

	UPROPERTY(EditAnywhere)
	float MaxForceFromPlayerHit{5000};

	UPROPERTY(EditAnywhere)
	float MinForceFromPlayerHit{500};

	UPROPERTY(EditAnywhere)
	float ImpactLocationZ{50};
private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UBoxComponent* Collision{nullptr};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent* IcePlane{nullptr};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent* IceBoundary{nullptr};
	
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

};
