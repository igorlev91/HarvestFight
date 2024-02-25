
#include "FertiliserSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/VFX/SquashAndStretch.h"

AFertiliserSpawner::AFertiliserSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InterfaceType = EInterfaceType::FertilizerBin;
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("Item Component"));
	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
	
	ChickenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chicken Mesh"));
	//ChickenMesh->AttachToComponent(ItemComponent->Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//ChickenMesh->SetRelativeLocation(FVector(75.006312,1.607227,167.150239)); 

	ItemComponent->Mesh->CustomDepthStencilValue = 1;
	ChickenMesh->CustomDepthStencilValue = 1;
}

void AFertiliserSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);

	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	
	ChickenMesh->AttachToComponent(ItemComponent->Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ChickenMesh->SetRelativeLocation(FVector(75.006312,1.607227,167.150239)); // Copied from the original blueprint placement
	SSComponent->SetMeshToStretch(ChickenMesh);

	if (!HasAuthority())
		return;
	
	SpawnTimer = SpawnInterval;

	//SSComponent->Disable();
}

void AFertiliserSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFertiliserSpawner, SpawnTimer);
	DOREPLIFETIME(AFertiliserSpawner, SpawnedFertiliser);
	DOREPLIFETIME(AFertiliserSpawner, bBagCountReachedMax);
}

void AFertiliserSpawner::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (SpawnedFertiliser != nullptr && SSComponent && SSComponent->IsActive() == false)
	{
		SSComponent->SetMeshesToStretch({ChickenMesh, SpawnedFertiliser->ItemComponent->Mesh});
		SSComponent->Enable();
	}
	else if (!SpawnedFertiliser&& SSComponent && SSComponent->IsActive() == true)
	{
		SSComponent->SetMeshesToStretch({ChickenMesh});
		SSComponent->Disable();
	}
	
	if (!HasAuthority())
		return;

	GenerateFertiliserOnTimer(_DeltaTime);
	CheckForTooManyFertiliserBags();
}

UStaticMeshComponent* AFertiliserSpawner::GetMesh()
{
	return ItemComponent->Mesh;
}

void AFertiliserSpawner::Interact(APrototype2Character* _Player)
{
	if (!SpawnedFertiliser)
		return;

	SpawnTimer = SpawnInterval;

	//SSComponent->Disable();
	
	_Player->PickupItem(SpawnedFertiliser, EPickupActor::FertilizerActor);
	SpawnedFertiliser = nullptr;
}

void AFertiliserSpawner::HoldInteract(APrototype2Character* _Player)
{
	
}

void AFertiliserSpawner::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner,
                                               int _PlayerID)
{
	_InvokingWidget->SetHUDInteractText("Gather");
	if (SpawnedFertiliser)
		SpawnedFertiliser->ItemComponent->Mesh->SetRenderCustomDepth(true);
}

bool AFertiliserSpawner::IsInteractable(APrototype2PlayerState* _Player)
{
	if (bBagCountReachedMax)
		return false;
	
	if (SpawnTimer <= 0.0f)
	{
		return true;
	}
	else
	{
		
		return false;
	}
		
}

void AFertiliserSpawner::ClientInteract(APrototype2Character* _Player)
{
	SSComponent->Disable();
	
	if (SpawnedFertiliser)
	{
		SpawnedFertiliser->Client_Pickup();
	}
}

void AFertiliserSpawner::OnClientWalkAway(APrototype2Character* _Player)
{
	if (SpawnedFertiliser)
		SpawnedFertiliser->ItemComponent->Mesh->SetRenderCustomDepth(false);
}

bool AFertiliserSpawner::HasSpawnedFruit()
{
	if (SpawnedFertiliser)
		return true;

	return false;
}

void AFertiliserSpawner::GenerateFertiliserOnTimer(float _DeltaTime)
{
	if (SpawnTimer > 0)
	{
		SpawnTimer -= _DeltaTime;

		if (SpawnTimer <= 0)
		{
			SpawnFertiliser();
		}
	}
}

void AFertiliserSpawner::SpawnFertiliser()
{
	if (!HasAuthority())
		return;

	if (!FertiliserPrefab)
		return;

	SpawnedFertiliser = GetWorld()->SpawnActor<AFertiliser>(FertiliserPrefab);
	SpawnedFertiliser->SetReplicates(true);
	Multi_OnSpawnFertiliser(SpawnedFertiliser->ItemComponent->Mesh);
	SpawnedFertiliser->ItemComponent->Mesh->SetSimulatePhysics(false);
	SpawnedFertiliser->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnedFertiliser->ItemComponent->Mesh->SetIsReplicated(false);
	//SpawnedFertiliser->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
	SpawnedFertiliser->ItemComponent->Multi_DisableCollisionAndAttach();
	if (FertiliserDatas.Num() > 0)
	{
		int8 ChanceOfGold = rand() % 2;
		if (ChanceOfGold == 0)
		{
			USeedData* FertiliserData = FertiliserDatas[ChanceOfGold];
			SpawnedFertiliser->SetSeedData(FertiliserData, EPickupActor::FertilizerActor);
		}
		else
		{
			USeedData* FertiliserData = FertiliserDatas[rand() % FertiliserDatas.Num()];
			SpawnedFertiliser->SetSeedData(FertiliserData, EPickupActor::FertilizerActor);
		}
	}
	SpawnedFertiliser->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SpawnedFertiliser->SetActorRelativeLocation({SpawnXPosition, 0.0f, SpawnZPosition});


}

void AFertiliserSpawner::Multi_OnSpawnFertiliser_Implementation(UStaticMeshComponent* _NewFertiliserMesh)
{
}

void AFertiliserSpawner::CheckForTooManyFertiliserBags()
{
	if (FertiliserDatas.Num() <= 0)
		return;

	if (!SpawnedFertiliser)
		return;

	if (!GhostMaterial)
		return;

	if (!SpawnedFertiliser->SeedData)
		return;

	if (SpawnedFertiliser->SeedData->PacketMaterials.Num() < 2)
		return;

	int32 UnclaimedBagCount{};
	TArray<AActor*> FoundBags{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFertiliser::StaticClass(), FoundBags);
	TArray<AActor*> OtherFertiliserSpawners{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFertiliserSpawner::StaticClass(), OtherFertiliserSpawners);
	for(auto FertiliserSpawnerActor : OtherFertiliserSpawners)
	{
		if (AFertiliserSpawner* FertiliserSpawner = Cast<AFertiliserSpawner>(FertiliserSpawnerActor))
		{
			if (FertiliserSpawner->HasSpawnedFruit())
				UnclaimedBagCount++;
		}
	}
	
	if ((FoundBags.Num() - UnclaimedBagCount) >= FertiliserDatas[0]->FertiliserData->MaxSpawnedAtAnyGivenTime)
	{
		bBagCountReachedMax = true;
		Multi_SetGhostMaterial(SpawnedFertiliser);
	}
	else
	{
		bBagCountReachedMax = false;
		Multi_SetFertilizerMaterial(SpawnedFertiliser);
	}
}

void AFertiliserSpawner::Multi_SetGhostMaterial_Implementation(AFertiliser* _Fertiliser)
{
	_Fertiliser->ItemComponent->Mesh->SetMaterial(0, GhostMaterial);
	_Fertiliser->ItemComponent->Mesh->SetMaterial(1, GhostMaterial);
}

void AFertiliserSpawner::Multi_SetFertilizerMaterial_Implementation(AFertiliser* _Fertiliser)
{
	_Fertiliser->ItemComponent->Mesh->SetMaterial(0, _Fertiliser->SeedData->PacketMaterials[0]);
	_Fertiliser->ItemComponent->Mesh->SetMaterial(1, _Fertiliser->SeedData->PacketMaterials[1]);
}

