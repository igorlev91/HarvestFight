
#include "SquashAndStretch.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/ItemComponent.h"

USquashAndStretch::USquashAndStretch()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USquashAndStretch::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner()->HasAuthority())
	{
		if (auto Owner = GetOwner())
		{
			Owner->SetOwner(UGameplayStatics::GetGameMode(GetWorld())->GetOwner());

			if (auto RootOwner = GetOwner()->GetAttachParentActor())
			{
				RootOwner->SetOwner(UGameplayStatics::GetGameMode(GetWorld())->GetOwner());
			}
		}
		
		FindMeshesToStretch();

		// Check
		if (BoingCurve)
		{
			FOnTimelineFloat TimelineUpdateEvent{};
			TimelineUpdateEvent.BindDynamic(this, &USquashAndStretch::OnBoingUpdate);
			BoingTimeline.AddInterpFloat(BoingCurve, TimelineUpdateEvent);
			BoingTimeline.SetPlayRate(BoingSpeed);
		}
	}
}


void USquashAndStretch::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

	if (GetOwner()->HasAuthority())
	{
		if (bDoOnce)
		{
			bDoOnce = false;
			FindMeshesToStretch();
		}
		
		BoingTimeline.TickTimeline(_DeltaTime);
	
		if (bShouldUpdate)
		{
			SquashAndStretch();
		}
	}
}

void USquashAndStretch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USquashAndStretch, StartingStaticScales);
	DOREPLIFETIME(USquashAndStretch, StartingSkeletalScales);
	DOREPLIFETIME(USquashAndStretch, bShouldUpdate);

	DOREPLIFETIME(USquashAndStretch, bDoOnce);
	DOREPLIFETIME(USquashAndStretch, StaticMeshes);
	DOREPLIFETIME(USquashAndStretch, SkeletalMeshes);
}

void USquashAndStretch::Enable()
{
	if (GetOwner()->HasAuthority() || GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Enable();
	}
}

void USquashAndStretch::Disable()
{
	if (GetOwner()->HasAuthority() || GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Disable();
	}
}

void USquashAndStretch::Boing()
{
	if (GetOwner()->HasAuthority() || GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Boing();
	}
}

void USquashAndStretch::SetMeshesToStretch(TArray<UStaticMeshComponent*> _Statics,
	TArray<USkeletalMeshComponent*> _Skeletons)
{
	if (GetOwner()->HasAuthority() || GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetMeshesToStretch(_Statics, _Skeletons);
	}
}

void USquashAndStretch::FindMeshesToStretch()
{
	StartingSkeletalScales.Empty();
	StartingStaticScales.Empty();
	SkeletalMeshes.Empty();
	StaticMeshes.Empty();
	
	if (auto Owner = GetOwner())
	{
		if (auto Mesh = Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
		{
			auto MeshIndex = SkeletalMeshes.Add(Cast<USkeletalMeshComponent>(Mesh));
			StartingSkeletalScales.Add(SkeletalMeshes[MeshIndex]->GetComponentScale());
		}
		if (auto Mesh = Owner->GetComponentByClass(UStaticMeshComponent::StaticClass()))
		{
			auto MeshIndex = StaticMeshes.Add(Cast<UStaticMeshComponent>(Mesh));
			StartingStaticScales.Add(StaticMeshes[MeshIndex]->GetComponentScale());
		}
	}
}

void USquashAndStretch::SquashAndStretch()
{
	Multi_SquashAndStretch(GetWorld()->GetTimeSeconds());
}

void USquashAndStretch::OnBoingUpdate(float _Value)
{
	Multi_BoingUpdate(_Value);
}

void USquashAndStretch::Multi_SquashAndStretch_Implementation(float _ServerCurrentTime)
{
	for(int32 i = 0; i < StaticMeshes.Num(); i++)
	{
		StaticMeshes[i]->SetWorldScale3D(StartingStaticScales[i] + (SSAxis * FMath::Sin(_ServerCurrentTime * SquashSpeed) * SquashMag));
	}
	for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
	{
		SkeletalMeshes[i]->SetWorldScale3D(StartingSkeletalScales[i] + (SSAxis * FMath::Sin(_ServerCurrentTime * SquashSpeed) * SquashMag));
	}
}

void USquashAndStretch::Server_SquashAndStretch_Implementation()
{
	Multi_SquashAndStretch(GetWorld()->GetTimeSeconds());
}

void USquashAndStretch::Multi_BoingUpdate_Implementation(float _Value)
{
	for(int32 i = 0; i < StaticMeshes.Num(); i++)
	{
		StaticMeshes[i]->SetWorldScale3D(StartingStaticScales[i] + (SSAxis * FMath::Sin(2 * PI * _Value) * BoingSquashMag));
	}
	for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
	{
		SkeletalMeshes[i]->SetWorldScale3D(StartingSkeletalScales[i] + (SSAxis * FMath::Sin(2 * PI * _Value) * BoingSquashMag));
	}
}

void USquashAndStretch::Server_BoingUpdate_Implementation(float _Value)
{
	Multi_BoingUpdate(_Value);
}

void USquashAndStretch::Server_SetMeshesToStretch_Implementation(const TArray<class UStaticMeshComponent*>& _Statics, const TArray<class USkeletalMeshComponent*>& _Skeletons)
{
	StaticMeshes.Empty();
	SkeletalMeshes.Empty();
	StartingStaticScales.Empty();
	StartingSkeletalScales.Empty();
	
	StaticMeshes = _Statics;
	for(auto Mesh : StaticMeshes)
	{
		StartingStaticScales.Add(Mesh->GetComponentScale());
	}
	SkeletalMeshes = _Skeletons;
	for(auto Mesh : SkeletalMeshes)
	{
		StartingSkeletalScales.Add(Mesh->GetComponentScale());
	}
}

void USquashAndStretch::Server_Boing_Implementation()
{
	BoingTimeline.PlayFromStart();
}

void USquashAndStretch::Server_Disable_Implementation()
{
	bShouldUpdate = false;
}

void USquashAndStretch::Server_Enable_Implementation()
{
	StartingStaticScales.Empty();
	StartingSkeletalScales.Empty();
	bShouldUpdate = true;
	
	for(auto StaticMesh : StaticMeshes)
	{
		StartingStaticScales.Add(StaticMesh->GetComponentScale());
	}
	for(auto SkeletalMesh : SkeletalMeshes)
	{
		StartingSkeletalScales.Add(SkeletalMesh->GetComponentScale());
	}
}

