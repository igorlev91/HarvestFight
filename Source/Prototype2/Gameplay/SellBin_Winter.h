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

public:
	void GetHit(float _AttackCharge, float _MxAttackCharge, FVector _AttackerLocation);
	
protected:
	virtual void BeginPlay() override;
	ASellBin_Winter();

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	void SetShippingBinPosition_Networked(FVector _Pos = FVector(-104.559325,-72.190911,-13.473242));

	UFUNCTION(Server, Reliable)
	void Server_DetachComponents(FVector _Pos);
	void Server_DetachComponents_Implementation(FVector _Pos);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DetachComponents(FVector _Pos);
	void Multi_DetachComponents_Implementation(FVector _Pos);
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float MaxForceFromPlayerHit{1800};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float MinForceFromPlayerHit{1000};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float ImpactLocationZ{50};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UBoxComponent* Collision{nullptr};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent* IcePlane{nullptr};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent* IceBoundary{nullptr};
};
