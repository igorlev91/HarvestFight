
#include "VFXComponent.h"

#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"

UVFXComponent::UVFXComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InitOwner();
		
	if (!Owner)
		return;

	if (!RootComponent)
		return;
	
	InitMeshes();
	InitNiagraComponents();
}

void UVFXComponent::OnComponentCreated()
{
	Super::OnComponentCreated();



}

void UVFXComponent::OnParentActorConstruction()
{

}

void UVFXComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UVFXComponent, WalkPoof_NiagaraComponent);
	DOREPLIFETIME(UVFXComponent, SprintPoof_NiagaraComponent);
	DOREPLIFETIME(UVFXComponent, Sweat_NiagaraComponent);
	DOREPLIFETIME(UVFXComponent, AttackTrail_NiagaraComponent);
	DOREPLIFETIME(UVFXComponent, Attack_NiagaraComponent);
	DOREPLIFETIME(UVFXComponent, Dizzy_NiagaraComponent);
}

void UVFXComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UVFXComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ToggleParticleSystems(); 
}

void UVFXComponent::InitNiagraComponents()
{
	if (!CastedOwner)
		return;
	
	if (CastedOwner->WeaponMesh)
	{
		Dizzy_NiagaraComponent = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(), TEXT("Dizzy_NiagaraComponent"));
		Dizzy_NiagaraComponent->RegisterComponent();
		Dizzy_NiagaraComponent->SetupAttachment(CastedOwner->GetMesh(), FName("Base-HumanHead"));
	}
	
	WalkPoof_NiagaraComponent = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(),TEXT("WalkPoof_NiagaraComponent"));
	WalkPoof_NiagaraComponent->RegisterComponent();
	WalkPoof_NiagaraComponent->SetupAttachment(RootComponent);
	
	SprintPoof_NiagaraComponent = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(),TEXT("SprintPoof_NiagaraComponent"));
	SprintPoof_NiagaraComponent->RegisterComponent();
	SprintPoof_NiagaraComponent->SetupAttachment(RootComponent);
	
	Sweat_NiagaraComponent = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(), TEXT("Sweat_NiagaraComponent"));
	Sweat_NiagaraComponent->RegisterComponent();
	Sweat_NiagaraComponent->SetupAttachment(RootComponent);
	
	if (CastedOwner->WeaponMesh)
	{
		AttackTrail_NiagaraComponent = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(),TEXT("AttackTrail_NiagaraComponent"));
		AttackTrail_NiagaraComponent->RegisterComponent();
		AttackTrail_NiagaraComponent->SetupAttachment(CastedOwner->WeaponMesh);
	}
	
	Attack_NiagaraComponent = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(),TEXT("Attack_NiagaraComponent"));
	Attack_NiagaraComponent->RegisterComponent();
	Attack_NiagaraComponent->SetupAttachment(RootComponent);
}

void UVFXComponent::ActivateParticleSystemFromEnum(EParticleSystems _NewSystem)
{
	if (!Owner)
		return;
	
	if (Owner->HasAuthority() || Owner->GetLocalRole() == ROLE_AutonomousProxy)
		ParticleSystemsToActivate.Add(_NewSystem);
}

void UVFXComponent::DeActivateParticleSystemFromEnum(EParticleSystems _NewSystem)
{
	if (!Owner)
		return;
	
	if (Owner->HasAuthority() || Owner->GetLocalRole() == ROLE_AutonomousProxy)
		ParticleSystemsToDeActivate.Add(_NewSystem);
}

void UVFXComponent::ToggleParticleSystems()
{
	if (!CastedOwner)
		return;
	
	if (!CastedOwner->HasIdealRole())
		return;
	
	if (ParticleSystemsToActivate.Num() > 0 || ParticleSystemsToDeActivate.Num() > 0)
	{
		Server_ToggleParticleSystems(ParticleSystemsToActivate, ParticleSystemsToDeActivate);
		ParticleSystemsToActivate.Empty();
		ParticleSystemsToDeActivate.Empty();
	}
}

void UVFXComponent::Server_ToggleParticleSystems_Implementation(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off)
{
	Multi_ToggleParticleSystems(_On, _Off);
}

void UVFXComponent::Multi_ToggleParticleSystems_Implementation(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off)
{
	for(auto ParticleSystemCast : _On)
	{
		switch (ParticleSystemCast)
		{
		case EParticleSystems::WalkPoof:
			{
				WalkPoof_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystems::SprintPoof:
			{
				SprintPoof_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystems::Sweat:
			{
				Sweat_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystems::AttackTrail:
			{
				AttackTrail_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystems::Attack:
			{
				Attack_NiagaraComponent->Activate(true);
				break;
			}
		case EParticleSystems::Test:
			{
				//Test_NiagraComponent->Activate();
				break;
			}
		default:
			break;
		}
	}

	for(auto ParticleSystemCast : _Off)
	{
		switch (ParticleSystemCast)
		{
		case EParticleSystems::WalkPoof:
			{
				WalkPoof_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystems::SprintPoof:
			{
				SprintPoof_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystems::Sweat:
			{
				Sweat_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystems::AttackTrail:
			{
				AttackTrail_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystems::Attack:
			{
				Attack_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystems::Test:
			{
				//Test_NiagraComponent->Deactivate();
				break;
			}
		default:
			break;
		}
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

void UVFXComponent::InitMeshes()
{
	// Area of attack indicator mesh set up
	AttackAreaIndicatorMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(),TEXT("AttackAreaIndicatorMesh"));
	AttackAreaIndicatorMesh->RegisterComponent();
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	AttackAreaIndicatorMesh->SetHiddenInGame(true);

}

