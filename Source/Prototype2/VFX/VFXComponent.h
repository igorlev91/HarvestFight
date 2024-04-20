
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void ActivateParticleSystem(UNiagaraComponent* _NewSystem);
	UFUNCTION()
	void DeActivateParticleSystem(UNiagaraComponent* _NewSystem);

	UFUNCTION(Server, Reliable)
	void Server_ActivateParticleSystem(UNiagaraComponent* _NewSystem);
	UFUNCTION(Server, Reliable)
	void Server_DeActivateParticleSystem(UNiagaraComponent* _NewSystem);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ActivateParticleSystem(UNiagaraComponent* _NewSystem);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DeActivateParticleSystem(UNiagaraComponent* _NewSystem);
	
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<class UNiagaraComponent*> ParticleSystemsToDeActivate{};
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<class UNiagaraComponent*> ParticleSystemsToActivate{};
	
	
	//UPROPERTY(VisibleAnywhere)
	//TArray<EParticleSystems> ParticleSystemsToActivate{};
	//UPROPERTY(VisibleAnywhere)
	//TArray<EParticleSystems> ParticleSystemsToDeActivate{};

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootComponent{nullptr};
	
	UPROPERTY(EditAnywhere)
	class AActor* Owner{nullptr};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Character* CastedOwner{nullptr};
	
	UFUNCTION()
	void ToggleParticleSystems();
	UFUNCTION()
	void InitOwner();

	UFUNCTION(Server, Unreliable)
	void Server_ToggleParticleSystems(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off);

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ToggleParticleSystems(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off);

	UPROPERTY(VisibleAnywhere)
	float ToggleDelay {0.3};
	UPROPERTY(EditAnywhere)
	float ToggleDelayTimer{};
};
