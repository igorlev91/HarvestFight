

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPadV2.generated.h"

UCLASS()
class PROTOTYPE2_API ALaunchPadV2 : public AActor
{
	GENERATED_BODY()

	ALaunchPadV2();
	void AssignComponentDefaults();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* _OverlappedComponent, 
					  AActor* _OtherActor, 
					  UPrimitiveComponent* _OtherComp, 
					  int32 _OtherBodyIndex, 
					  bool _bFromSweep, 
					  const FHitResult& _SweepResult );

	void Launch(class APickUpItem* _Item);
	void Launch(class APrototype2Character* _Player);
	
private: /* COMPONENTS */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	class USphereComponent* LaunchCollider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMeshComponent* LaunchPadMesh;

	UPROPERTY(VisibleAnywhere)
	class USquashAndStretch* SquashAndStretch{nullptr};

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* LaunchDirection{nullptr};

protected: /* VARIABLES */
	UPROPERTY(EditAnywhere)
	bool bLaunchWithArrowDirection{true};

	UPROPERTY(EditAnywhere)
	bool bForceVertical{false};
	
	UPROPERTY(EditAnywhere, Category="Strength")
	float ForwardStrength = 1500.0f;

	UPROPERTY(EditAnywhere, Category="Strength")
	float VerticalStrength = 1500.0f;
	
protected: /* AUDIO */
	void PlayLaunchSound();
	
	UPROPERTY(EditAnywhere)
	class USoundCue* LaunchCue{nullptr};
	UPROPERTY(EditAnywhere)
	class USoundAttenuation* LaunchAttenuation{nullptr};

};
