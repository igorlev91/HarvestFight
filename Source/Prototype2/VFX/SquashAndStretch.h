
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "SquashAndStretch.generated.h"

class APickUpItem;
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

	void SetMeshesToStretch(TArray<class UStaticMeshComponent*> _Statics = {});
	void SetMeshToStretch(class UStaticMeshComponent* _StaticMesh);
	void FindMeshesToStretch();

	bool IsActive() const;

	UPROPERTY(VisibleAnywhere)
	class APickUpItem* OwningItem;
protected:
	void SquashAndStretch();

	UFUNCTION()
	void OnBoingUpdate(float _Value);

	UFUNCTION()
	void OnBoingFinish();
	
	UFUNCTION(Server, Unreliable)
	void Server_Enable();

	UFUNCTION(Server, Unreliable)
	void Server_Disable();

	UFUNCTION(Server, Reliable)
	void Server_Boing();

	UFUNCTION(Server, Reliable)
	void Server_SetMeshesToStretch(const TArray<class UStaticMeshComponent*>& _Statics);
	UFUNCTION(Server, Reliable)
	void Server_SetMeshToStretch(class UStaticMeshComponent* _StaticMesh);

	UFUNCTION(Server, Reliable)
	void Server_BoingUpdate(float _Value);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_BoingUpdate(float _Value);

	UFUNCTION(Server, Unreliable)
	void Server_SquashAndStretch();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SquashAndStretch(float _ServerCurrentTime);


	UPROPERTY(VisibleAnywhere)
	class ASellBin* OwningSellBin;
	UPROPERTY(VisibleAnywhere)
	class AGrowSpot* OwningGrowSpot;
	UPROPERTY(VisibleAnywhere)
	class AFertiliserSpawner* OwningFertiliserSpawner;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<class UStaticMeshComponent*> StaticMeshes{};
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<FVector> StartScales{};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector SSAxis{1,1,-1};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SquashSpeed{0.2f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float BoingSpeed{0.8f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float SquashMag{0.1f};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float BoingSquashMag{0.3f};

	UPROPERTY(VisibleAnywhere)
	FTimeline BoingTimeline;
	
	UPROPERTY()
	AActor* ThisOwner{nullptr};
	


	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	class UCurveFloat* BoingCurve{};

	UPROPERTY(meta = (AllowPrivateAccess))
	bool bDoOnce{true};
};
