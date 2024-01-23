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
	ARadialPlot();
	
	void SetPlayerID(FString _Id);
	FString GetPlayerID() const;

	void SpawnGrowSpots(FString _PlayerName);

	void UpdateBeehiveFlowers();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Unreliable, meta = (AllowPrivateAccess))
	void Multi_SetPlotMaterial(APrototype2PlayerState* _Id);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	TArray<class AGrowSpot*> GrowSpots;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UMaterialInstance* PlotSignMaterial;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	UMaterialInstanceDynamic* PlotSignMaterialDynamic;

	UPROPERTY(Replicated, EditAnywhere, meta = (AllowPrivateAccess))
	class APlotSign* PlotSign;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<APlotSign> PlotSignPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<AGrowSpot> GrowSpotPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FString PlayerName;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float PlotSpread {180.0f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float PlotZHeight {30.0f};
	


	bool bDoOnce{true};
};
