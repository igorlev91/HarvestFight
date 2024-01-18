
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BobTransformComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE2_API UBobTransformComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetEnabled(bool _bEnabled);

private:
	UBobTransformComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UFUNCTION()
	virtual void BobParentOnTick(float _DeltaTime);

	UPROPERTY(EditAnywhere)
	FVector BobAmplitude;

	UPROPERTY(EditAnywhere)
	float BobSpeed;

	UPROPERTY(EditAnywhere)
	bool bIsBobbing{};

	UPROPERTY()
	AActor* Owner;

	UPROPERTY()
	class UStaticMeshComponent* OwnerMesh;

	UPROPERTY(Replicated)
	float StartTime{};
};
