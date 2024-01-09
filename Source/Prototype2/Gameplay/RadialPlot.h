#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RadialPlot.generated.h"

class APrototype2PlayerState;
UCLASS()
class PROTOTYPE2_API ARadialPlot : public AActor
{
	GENERATED_BODY()
	
public:
	void SetPlayerID(int32 _Id);
	int32 GetPlayerID() const;

	void UpdateBeehiveFlowers();

protected:
	ARadialPlot();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable, meta = (AllowPrivateAccess))
	void Multi_SetPlotMaterial(APrototype2PlayerState* _Id);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	TArray<class AGrowSpot*> GrowSpots;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UMaterialInstance* PlotSignMaterial;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	UMaterialInstanceDynamic* PlotSignMaterialDynamic;

	UPROPERTY(Replicated, EditAnywhere, meta = (AllowPrivateAccess))
	class UStaticMeshComponent* PlotSignMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<AGrowSpot> GrowSpotPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 Player_ID;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float PlotSpread {180.0f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float PlotZHeight {-20.0f};
	
	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;

	bool bDoOnce{true};
};
