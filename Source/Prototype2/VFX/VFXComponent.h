
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
	void ActivateParticleSystemFromEnum(UNiagaraComponent* _NewSystem);
	UFUNCTION()
	void DeActivateParticleSystemFromEnum(UNiagaraComponent* _NewSystem);

	UFUNCTION(Server, Reliable)
	void Server_ActivateParticleSystemFromEnum(UNiagaraComponent* _NewSystem);
	UFUNCTION(Server, Reliable)
	void Server_DeActivateParticleSystemFromEnum(UNiagaraComponent* _NewSystem);

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

	UFUNCTION(Server, Reliable)
	void Server_ToggleParticleSystems(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleParticleSystems(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off);
};
