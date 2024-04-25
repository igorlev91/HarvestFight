
#include "DataAssetWorldOverride.h"

#include "WorldOverrideData.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ADataAssetWorldOverride::ADataAssetWorldOverride()
{
	PrimaryActorTick.bCanEverTick = false;

	PreGameArenaSpawnLocations.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Default PregameArena")));
	PreGameArenaSpawnLocations.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Team One PregameArena")));
	PreGameArenaSpawnLocations.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Team Two PregameArena")));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PreGameArenaMesh(TEXT("/Game/Environment/SM_ShippingCrate"));
	if (PreGameArenaMesh.Object != nullptr)
	{
		for(auto PreGameArena : PreGameArenaSpawnLocations)
		{
			PreGameArena->SetStaticMesh(PreGameArenaMesh.Object);
		}
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PreGameArenaMaterial(TEXT("/Game/Materials/MI_PreGameArena"));
	if (PreGameArenaMaterial.Object != nullptr)
	{
		PreGameArenaSpawnLocations[0]->SetMaterial(0, PreGameArenaMaterial.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PreGameArenaMaterialTA(TEXT("/Game/Materials/MI_PreGameArena_TeamOne"));
	if (PreGameArenaMaterialTA.Object != nullptr)
	{
		PreGameArenaSpawnLocations[1]->SetMaterial(0, PreGameArenaMaterialTA.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PreGameArenaMaterialTT(TEXT("/Game/Materials/MI_PreGameArena_TeamTwo"));
	if (PreGameArenaMaterialTT.Object != nullptr)
	{
		PreGameArenaSpawnLocations[2]->SetMaterial(0, PreGameArenaMaterialTT.Object);
	}
	
	for(auto PreGameArena : PreGameArenaSpawnLocations)
	{
		PreGameArena->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PreGameArena->SetHiddenInGame(true);
		PreGameArena->SetWorldScale3D({5.0f, 5.0f, 5.0f});
	}
	PreGameArenaSpawnLocations[0]->SetWorldLocation({0.0f, 0.0f, 900});
	PreGameArenaSpawnLocations[1]->SetWorldLocation({700.0f, 0.0f, 900});
	PreGameArenaSpawnLocations[2]->SetWorldLocation({-700.0f, 0.0f, 900});

}

void ADataAssetWorldOverride::BeginPlay()
{
	Super::BeginPlay();

	if (WorldOverrideData)
	{
		if ( WorldOverrideData->Ambience)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), WorldOverrideData->Ambience);
		}
	}
}
