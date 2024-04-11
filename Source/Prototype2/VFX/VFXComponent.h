











	
	
	
	
	
	//TArray<EParticleSystems> ParticleSystemsToActivate{};
	//TArray<EParticleSystems> ParticleSystemsToDeActivate{};
	//UPROPERTY(VisibleAnywhere)
	//UPROPERTY(VisibleAnywhere)
	class AActor* Owner{nullptr};
	class APrototype2Character* CastedOwner{nullptr};
	class USceneComponent* RootComponent{nullptr};
	float ToggleDelay {0.3};
	float ToggleDelayTimer{};
	GENERATED_BODY()
	TArray<class UNiagaraComponent*> ParticleSystemsToActivate{};
	TArray<class UNiagaraComponent*> ParticleSystemsToDeActivate{};
	UFUNCTION()
	UFUNCTION()
	UFUNCTION()
	UFUNCTION()
	UFUNCTION(NetMulticast, Reliable)
	UFUNCTION(NetMulticast, Reliable)
	UFUNCTION(NetMulticast, Unreliable)
	UFUNCTION(Server, Reliable)
	UFUNCTION(Server, Reliable)
	UFUNCTION(Server, Unreliable)
	UPROPERTY(EditAnywhere)
	UPROPERTY(EditAnywhere)
	UPROPERTY(Replicated, VisibleAnywhere)
	UPROPERTY(Replicated, VisibleAnywhere)
	UPROPERTY(VisibleAnywhere)
	UPROPERTY(VisibleAnywhere)
	UPROPERTY(VisibleAnywhere)
	UVFXComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ActivateParticleSystem(UNiagaraComponent* _NewSystem);
	void DeActivateParticleSystem(UNiagaraComponent* _NewSystem);
	void InitOwner();
	void Multi_ActivateParticleSystem(UNiagaraComponent* _NewSystem);
	void Multi_DeActivateParticleSystem(UNiagaraComponent* _NewSystem);
	void Multi_ToggleParticleSystems(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off);
	void Server_ActivateParticleSystem(UNiagaraComponent* _NewSystem);
	void Server_DeActivateParticleSystem(UNiagaraComponent* _NewSystem);
	void Server_ToggleParticleSystems(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off);
	void ToggleParticleSystems();
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "VFXComponent.generated.h"
#pragma once
class PROTOTYPE2_API UVFXComponent : public UActorComponent
public:
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
{
};