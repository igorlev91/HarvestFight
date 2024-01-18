
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VFXComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE2_API UVFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVFXComponent();
	virtual void OnComponentCreated() override;
	UFUNCTION(BlueprintCallable)
	void OnParentActorConstruction();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void ActivateParticleSystemFromEnum(EParticleSystems _NewSystem);
	UFUNCTION()
	void DeActivateParticleSystemFromEnum(EParticleSystems _NewSystem);
	
	UPROPERTY(EditAnywhere, Category = VFX) 
	class UStaticMeshComponent* AttackAreaIndicatorMesh;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* WalkPoof_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* SprintPoof_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Sweat_NiagaraComponent;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* AttackTrail_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Attack_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Dizzy_NiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystems> ParticleSystemsToActivate{};
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystems> ParticleSystemsToDeActivate{};

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootComponent{nullptr};
	
	UPROPERTY(EditAnywhere)
	class AActor* Owner{nullptr};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Character* CastedOwner{nullptr};
	
	UFUNCTION()
	void InitNiagraComponents();
	UFUNCTION()
	void ToggleParticleSystems();
	UFUNCTION()
	void InitOwner();
	UFUNCTION()
	void InitMeshes();

	UFUNCTION(Server, Reliable)
	void Server_ToggleParticleSystems(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleParticleSystems(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off);
};
