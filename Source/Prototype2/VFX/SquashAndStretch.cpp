
#include "SquashAndStretch.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/GrowSpotData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/DataAssets/SellBinData.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/Gameplay/PlotSign.h"
#include "Prototype2/Gameplay/SellBin.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2/Spawning/FertiliserSpawner.h"

USquashAndStretch::USquashAndStretch()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> LinearCurveClass(TEXT("/Game/Curves/LinearCurve.LinearCurve"));
	if (LinearCurveClass.Succeeded())
	{
		BoingCurve = LinearCurveClass.Object;
	}
}

void USquashAndStretch::BeginPlay()
{
	Super::BeginPlay();
	
	ThisOwner = GetOwner();

	TArray<UActorComponent*> FoundComponent{};
	if (APickUpItem* SomeItem = Cast<APickUpItem>(ThisOwner))
		OwningItem = SomeItem;

	if (ASellBin* SomeSellBin = Cast<ASellBin>(ThisOwner))
		OwningSellBin = SomeSellBin;

	if (AGrowSpot* SomeGrowspot = Cast<AGrowSpot>(ThisOwner))
		OwningGrowSpot = SomeGrowspot;

	if (AFertiliserSpawner* SomeFertilizerSpawner = Cast<AFertiliserSpawner>(ThisOwner))
		OwningFertiliserSpawner = SomeFertilizerSpawner;
	
	FindMeshesToStretch();

	//
	if (BoingCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("OnBoingUpdate"));
		BoingTimeline.AddInterpFloat(BoingCurve, TimelineCallback);
		BoingTimeline.SetPlayRate(BoingSpeed);

		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("OnBoingFinish"));
		BoingTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}


void USquashAndStretch::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);
	
	BoingTimeline.TickTimeline(_DeltaTime);
}

void USquashAndStretch::Enable()
{

	if (!BoingTimeline.IsPlaying())
	{
		BoingTimeline.SetLooping(true);
		BoingTimeline.Play();
	}
}

void USquashAndStretch::Disable()
{
	if (BoingTimeline.IsPlaying())
	{
		BoingTimeline.SetNewTime(0.0f);
		BoingTimeline.SetLooping(false);
		BoingTimeline.Stop();

		BoingTimeline.SetLooping(false);
	}
	
}

void USquashAndStretch::Boing()
{
	if (BoingTimeline.IsPlaying())
	{
		BoingTimeline.SetNewTime(0.0f);
		BoingTimeline.SetLooping(false);
		BoingTimeline.Stop();

		
	}
	BoingTimeline.SetLooping(false);
	BoingTimeline.PlayFromStart();
}

void USquashAndStretch::SetMeshesToStretch(TArray<UStaticMeshComponent*> _Statics)
{
	OwningItem = nullptr;
	StaticMeshes.Empty();
	StartScales.Empty();
	for(auto Mesh : _Statics)
	{
		StaticMeshes.Add(Mesh);
		StartScales.Add(Mesh->GetComponentScale());
	}
		
}

void USquashAndStretch::SetMeshToStretch(UStaticMeshComponent* _StaticMesh)
{
	if (IsValid(_StaticMesh))
	{
		OwningItem = nullptr;
		StaticMeshes.Empty();
		StartScales.Empty();
		StaticMeshes.Add(_StaticMesh);
		StartScales.Add(_StaticMesh->GetComponentScale());
	}
}

void USquashAndStretch::FindMeshesToStretch()
{
	StaticMeshes.Empty();
	StartScales.Empty();
	if (OwningItem)
	{
		StaticMeshes.Add(Cast<UStaticMeshComponent>(OwningItem->ItemComponent->Mesh));
		StartScales.Add(OwningItem->ItemComponent->Mesh->GetComponentScale());
	}
}

bool USquashAndStretch::IsActive() const
{
	return BoingTimeline.IsPlaying();
}

void USquashAndStretch::SquashAndStretch()
{
	Server_SquashAndStretch();
}

void USquashAndStretch::OnBoingUpdate(float _Value)
{
	if (OwningItem != nullptr)
	{
		if (OwningItem->ServerData.SeedData)
		{
			if (OwningItem->ServerData.SeedData->BabyType == EPickupDataType::BeehiveData)
				OwningItem->ItemComponent->Mesh->SetWorldScale3D(FVector{2.0f,2.0f,2.0f} + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
			else
				OwningItem->ItemComponent->Mesh->SetWorldScale3D(OwningItem->ServerData.SeedData->BabyScale + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
		}
		return;
	}
	if (OwningSellBin != nullptr)
	{
		if (OwningSellBin->SellBinData)
		{
			for(int32 i = 0; i  < StaticMeshes.Num(); i++)
			{
				if (OwningSellBin->SellBinData)
				{
					if (IsValid(StaticMeshes[i]))
						StaticMeshes[i]->SetWorldScale3D(OwningSellBin->SellBinData->DesiredScale + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
				}
			}
		}
		return;
	}
	if (OwningGrowSpot != nullptr)
	{
		if (OwningGrowSpot->GrowSpotData)
		{
			for(int32 i = 0; i  < StaticMeshes.Num(); i++)
			{
				if (IsValid(StaticMeshes[i]))
					if (OwningGrowSpot->GrowSpotData)
						StaticMeshes[i]->SetWorldScale3D(OwningGrowSpot->GrowSpotData->DesiredScale + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
			}
		}
		return;
	}
	if (OwningFertiliserSpawner != nullptr)
	{
		StaticMeshes[0]->SetWorldScale3D(StartScales[0] + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
		if (IsValid(OwningFertiliserSpawner->SpawnedFertiliser))
		{
			if (IsValid(OwningFertiliserSpawner->SpawnedFertiliser->ServerData.SeedData))
			{
				
				for(int32 i = 1; i  < StaticMeshes.Num(); i++)
				{
					if (IsValid(StaticMeshes[i]))
						StaticMeshes[i]->SetWorldScale3D(OwningFertiliserSpawner->SpawnedFertiliser->ServerData.SeedData->BabyScale + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
				}
			}
		}
		return;
	}

	for(int32 i = 0; i  < StaticMeshes.Num(); i++)
	{
		if (IsValid(StaticMeshes[i]))
			StaticMeshes[i]->SetWorldScale3D(StartScales[i] + (SSAxis * (FMath::Sin(2 * PI * _Value) * BoingSquashMag)));
	}
}

void USquashAndStretch::OnBoingFinish()
{
}

void USquashAndStretch::Server_SetMeshToStretch_Implementation(UStaticMeshComponent* _StaticMesh)
{
	StaticMeshes.Empty();
	StaticMeshes.Add(_StaticMesh);
}

void USquashAndStretch::Multi_SquashAndStretch_Implementation(float _ServerCurrentTime)
{

}

void USquashAndStretch::Server_SquashAndStretch_Implementation()
{

}

void USquashAndStretch::Multi_BoingUpdate_Implementation(float _Value)
{
	for(int32 i = 0; i < StaticMeshes.Num(); i++)
	{
		StaticMeshes[i]->SetWorldScale3D(FVector::One() + (SSAxis * FMath::Sin(2 * PI * _Value) * BoingSquashMag));
	}
	
	if (OwningItem)
	{
		for(int32 i = 0; i < StaticMeshes.Num(); i++)
		{
			if (OwningItem->ServerData.SeedData)
			{
				StaticMeshes[i]->SetWorldScale3D(OwningItem->ServerData.SeedData->BabyScale + (SSAxis * FMath::Sin(2 * PI * _Value) * BoingSquashMag));
			}
		}
	}
}

void USquashAndStretch::Server_BoingUpdate_Implementation(float _Value)
{
	for(int32 i = 0; i < StaticMeshes.Num(); i++)
	{
		StaticMeshes[i]->SetWorldScale3D(OwningItem->ServerData.SeedData->BabyScale + (SSAxis * FMath::Sin(2 * PI * _Value) * BoingSquashMag));
	}
}

void USquashAndStretch::Server_SetMeshesToStretch_Implementation(const TArray<class UStaticMeshComponent*>& _Statics)
{
	StaticMeshes.Empty();
	for(UStaticMeshComponent* Mesh : _Statics)
		StaticMeshes.Add(Mesh);
	
	OwningItem = nullptr;
}

void USquashAndStretch::Server_Boing_Implementation()
{
	
}

void USquashAndStretch::Server_Disable_Implementation()
{
}

void USquashAndStretch::Server_Enable_Implementation()
{
}

