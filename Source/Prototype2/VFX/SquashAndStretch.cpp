
#include "SquashAndStretch.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/ItemComponent.h"

USquashAndStretch::USquashAndStretch()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USquashAndStretch::BeginPlay()
{
	Super::BeginPlay();
	
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


void USquashAndStretch::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
	
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

void USquashAndStretch::Enable()
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

void USquashAndStretch::Disable()
{
	bShouldUpdate = false;
}

void USquashAndStretch::Boing()
{
	BoingTimeline.PlayFromStart();
}

void USquashAndStretch::SetMeshesToStretch(TArray<UStaticMeshComponent*> _Statics,
	TArray<USkeletalMeshComponent*> _Skeletons)
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
	for(int32 i = 0; i < StaticMeshes.Num(); i++)
	{
		StaticMeshes[i]->SetWorldScale3D(StartingStaticScales[i] + (SSAxis * FMath::Sin(GetWorld()->GetTimeSeconds() * SquashSpeed) * SquashMag));
	}
	for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
	{
		SkeletalMeshes[i]->SetWorldScale3D(StartingSkeletalScales[i] + (SSAxis * FMath::Sin(GetWorld()->GetTimeSeconds() * SquashSpeed) * SquashMag));
	}
}

void USquashAndStretch::OnBoingUpdate(float _Value)
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

