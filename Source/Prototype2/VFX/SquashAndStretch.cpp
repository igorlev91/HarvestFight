
#include "SquashAndStretch.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Pickups/PickUpItem.h"

USquashAndStretch::USquashAndStretch()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
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
	DOREPLIFETIME(USquashAndStretch, bShouldUpdate);
	DOREPLIFETIME(USquashAndStretch, OwningPickupItem);
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
	SkeletalMeshes.Empty();
	StaticMeshes.Empty();
	
	if (auto Owner = GetOwner())
	{
		OwningPickupItem = Cast<APickUpItem>(Owner);
		if (auto Mesh = Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
		{
			auto MeshIndex = SkeletalMeshes.Add(Cast<USkeletalMeshComponent>(Mesh));
		}
		if (auto Mesh = Owner->GetComponentByClass(UStaticMeshComponent::StaticClass()))
		{
			auto MeshIndex = StaticMeshes.Add(Cast<UStaticMeshComponent>(Mesh));
		}
	}
}

void USquashAndStretch::SquashAndStretch()
{
	//Server_SquashAndStretch();
}

void USquashAndStretch::OnBoingUpdate(float _Value)
{
	Server_BoingUpdate(_Value);
}

void USquashAndStretch::Multi_SquashAndStretch_Implementation(float _ServerCurrentTime)
{
	if (OwningPickupItem)
	{
		
		for(int32 i = 0; i < StaticMeshes.Num(); i++)
		{
			StaticMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(_ServerCurrentTime * SquashSpeed) * 0.2f));
		}
		for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
		{
			SkeletalMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(_ServerCurrentTime * SquashSpeed) * 0.2f));
		}
	}
	
}

void USquashAndStretch::Server_SquashAndStretch_Implementation()
{
	if (OwningPickupItem)
	{
		
		for(int32 i = 0; i < StaticMeshes.Num(); i++)
		{
			StaticMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(GetWorld()->GetTimeSeconds() * SquashSpeed) * 0.2f));
		}
		for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
		{
			SkeletalMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(GetWorld()->GetTimeSeconds() * SquashSpeed) * 0.2f));
		}
	}
}

void USquashAndStretch::Multi_BoingUpdate_Implementation(float _Value)
{
	if (OwningPickupItem)
	{
		
		for(int32 i = 0; i < StaticMeshes.Num(); i++)
		{
			StaticMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(2 * PI * _Value) * 0.2f));
		}
		for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
		{
			SkeletalMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(2 * PI * _Value) * 0.2f));
		}
	}

}

void USquashAndStretch::Server_BoingUpdate_Implementation(float _Value)
{
	if (OwningPickupItem)
	{
		
		for(int32 i = 0; i < StaticMeshes.Num(); i++)
		{
			StaticMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(2 * PI * _Value) * 0.2f));
		}
		for(int32 i = 0; i < SkeletalMeshes.Num(); i++)
		{
			SkeletalMeshes[i]->SetWorldScale3D(OwningPickupItem->PlantData->PlantScale + (SSAxis * FMath::Sin(2 * PI * _Value) * 0.2f));
		}
	}
}

void USquashAndStretch::Server_SetMeshesToStretch_Implementation(const TArray<class UStaticMeshComponent*>& _Statics, const TArray<class USkeletalMeshComponent*>& _Skeletons)
{
	StaticMeshes.Empty();
	SkeletalMeshes.Empty();
	StaticMeshes = _Statics;
	SkeletalMeshes = _Skeletons;
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
	bShouldUpdate = true;
}

