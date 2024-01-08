
#include "SquashAndStretch.h"

#include "Prototype2/Pickups/ItemComponent.h"

USquashAndStretch::USquashAndStretch()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USquashAndStretch::BeginPlay()
{
	Super::BeginPlay();
	
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
		if (auto Item = Owner->GetComponentByClass(UItemComponent::StaticClass()))
		{
			if (auto ItemComponent = Cast<UItemComponent>(Item))
			{
				auto MeshIndex = StaticMeshes.Add(ItemComponent->Mesh);
				StartingStaticScales.Add(StaticMeshes[MeshIndex]->GetComponentScale());
			}
		}
	}
}


void USquashAndStretch::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
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

