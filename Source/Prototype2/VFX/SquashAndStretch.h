
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "SquashAndStretch.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE2_API USquashAndStretch : public UActorComponent
{
	GENERATED_BODY()
	
	USquashAndStretch();
	virtual void BeginPlay() override;
	virtual void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;

public:
	void Enable();
	void Disable();
	void Boing();

	void SetMeshesToStretch(TArray<class UStaticMeshComponent*> _Statics = {}, TArray<class USkeletalMeshComponent*> _Skeletons = {});
	void FindMeshesToStretch();
		
protected:
	void SquashAndStretch();

	UFUNCTION()
	void OnBoingUpdate(float _Value);

	UPROPERTY(EditAnywhere)
	bool bShouldUpdate{false};
	
	UPROPERTY(meta = (AllowPrivateAccess))
	TArray<class UStaticMeshComponent*> StaticMeshes{};
	UPROPERTY(meta = (AllowPrivateAccess))
	TArray<class USkeletalMeshComponent*> SkeletalMeshes{};

	UPROPERTY(meta = (AllowPrivateAccess))
	TArray<FVector> StartingStaticScales{};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector SSAxis{1,1,-1};
	
	UPROPERTY(meta = (AllowPrivateAccess))
	TArray<FVector> StartingSkeletalScales{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SquashSpeed{10};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float BoingSpeed{2};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SquashMag{0.1f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float BoingSquashMag{0.2f};

	UPROPERTY()
	FTimeline BoingTimeline;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UCurveFloat* BoingCurve{};

	bool bDoOnce{true};
};
