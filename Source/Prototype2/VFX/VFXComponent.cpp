
#include "VFXComponent.h"

#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"

UVFXComponent::UVFXComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	///InitMeshes();


		

}

void UVFXComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVFXComponent, ParticleSystemsToDeActivate);
	DOREPLIFETIME(UVFXComponent, ParticleSystemsToActivate);
}

void UVFXComponent::BeginPlay()
{
	Super::BeginPlay();

	InitOwner();
}

void UVFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Owner && Owner->HasAuthority())
		ToggleParticleSystems(); 
	/*if (ToggleDelayTimer > 0)
	{
		ToggleDelayTimer -= DeltaTime;
		if (ToggleDelayTimer <= 0)
		{

		}
	}*/
}

void UVFXComponent::ActivateParticleSystemFromEnum(UNiagaraComponent* _NewSystem)
{
	if (CastedOwner)
	{
		if (!CastedOwner->HasIdealRole())
			return;
	}

	Server_ActivateParticleSystemFromEnum(_NewSystem);
}

void UVFXComponent::DeActivateParticleSystemFromEnum(UNiagaraComponent* _NewSystem)
{
	Server_DeActivateParticleSystemFromEnum(_NewSystem);
}

void UVFXComponent::Server_ActivateParticleSystemFromEnum_Implementation(UNiagaraComponent* _NewSystem)
{
	ParticleSystemsToActivate.Add(_NewSystem);
}

void UVFXComponent::Server_DeActivateParticleSystemFromEnum_Implementation(UNiagaraComponent* _NewSystem)
{
	ParticleSystemsToDeActivate.Add(_NewSystem);
}

void UVFXComponent::ToggleParticleSystems()
{
	if (ParticleSystemsToActivate.Num() > 0 || ParticleSystemsToDeActivate.Num() > 0)
	{
		for(auto ParticleSystemCast : ParticleSystemsToActivate)
		{
			if (ParticleSystemCast)
				ParticleSystemCast->Activate();
		}

		for(auto ParticleSystemCast : ParticleSystemsToDeActivate)
		{
			if (ParticleSystemCast)
				ParticleSystemCast->Deactivate();
		}
		ParticleSystemsToActivate.Empty();
		ParticleSystemsToDeActivate.Empty();
	}
}

void UVFXComponent::Server_ToggleParticleSystems_Implementation(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off)
{
	Multi_ToggleParticleSystems(_On, _Off);
}

void UVFXComponent::Multi_ToggleParticleSystems_Implementation(const TArray<UNiagaraComponent*>& _On, const TArray<UNiagaraComponent*>& _Off)
{
	for(auto ParticleSystemCast : _On)
	{
		if (ParticleSystemCast)
			ParticleSystemCast->Activate();
	}

	for(auto ParticleSystemCast : _Off)
	{
		if (ParticleSystemCast)
			ParticleSystemCast->Deactivate();
	}
}

void UVFXComponent::InitOwner()
{
	Owner = GetOwner();
	
	if (!Owner)
		return;

	RootComponent = Owner->GetRootComponent();
	CastedOwner = Cast<APrototype2Character>(Owner);
}

