
#pragma once

#include "CoreMinimal.h"
#include "GrowSpot.h"
#include "AutoGrowSpot.generated.h"

UCLASS()
class PROTOTYPE2_API AAutoGrowSpot : public AGrowSpot
{
	GENERATED_BODY()

public:
	AAutoGrowSpot();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;

	UPROPERTY(EditAnywhere)
	float ReGrowInterval{3.0f};
	UPROPERTY(VisibleAnywhere, Replicated)
	float ReGrowTimer{};
};
