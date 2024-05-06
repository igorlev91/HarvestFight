
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

	//if (Owner && Owner->HasAuthority())
	//	ToggleParticleSystems(); 
	/*if (ToggleDelayTimer > 0)
	{
		ToggleDelayTimer -= DeltaTime;
		if (ToggleDelayTimer <= 0)
		{

		}
	}*/
}

void UVFXComponent::ActivateParticleSystem(UNiagaraComponent* _NewSystem)
{
	if (GetOwner() == nullptr || _NewSystem == nullptr)
		return;
	
	if (GetOwner()->HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Server | Activated Particle System: %s"), *_NewSystem->GetFName().ToString());
		Multi_ActivateParticleSystem(_NewSystem);
	}
	else if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Client | Activated Particle System: %s"), *_NewSystem->GetFName().ToString());
		Server_ActivateParticleSystem(_NewSystem);
	}
}

void UVFXComponent::DeActivateParticleSystem(UNiagaraComponent* _NewSystem)
{
	if (GetOwner() == nullptr || _NewSystem == nullptr)
		return;
	
	if (GetOwner()->HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Server | Deactivate Particle System: %s"), *_NewSystem->GetFName().ToString());
		Multi_DeActivateParticleSystem(_NewSystem);
	}
	else if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Client | Deactivate Particle System: %s"), *_NewSystem->GetFName().ToString());
		Server_DeActivateParticleSystem(_NewSystem);
	}
}

void UVFXComponent::Multi_DeActivateParticleSystem_Implementation(UNiagaraComponent* _NewSystem)
{
	_NewSystem->Deactivate();
}

void UVFXComponent::Multi_ActivateParticleSystem_Implementation(UNiagaraComponent* _NewSystem)
{
	_NewSystem->Activate();
}

void UVFXComponent::Server_ActivateParticleSystem_Implementation(UNiagaraComponent* _NewSystem)
{
	Multi_ActivateParticleSystem(_NewSystem);
}

void UVFXComponent::Server_DeActivateParticleSystem_Implementation(UNiagaraComponent* _NewSystem)
{
	Multi_DeActivateParticleSystem(_NewSystem);
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

