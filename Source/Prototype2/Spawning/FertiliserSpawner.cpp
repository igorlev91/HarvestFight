
#include "FertiliserSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
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
}

void AFertiliserSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFertiliserSpawner, SpawnedFertiliser);
}

void AFertiliserSpawner::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	// LOCALIZED
	if (!IsValid(GameStateRef))
	{
		AGameStateBase* SomeGamestate = UGameplayStatics::GetGameState(GetWorld());
		if (IsValid(SomeGamestate))
		{
			GameStateRef = Cast<APrototype2Gamestate>(SomeGamestate);
		}
	}

	// SERVER ONLY
	if (HasAuthority())
		GenerateFertiliserOnTimer(_DeltaTime);

	// LOACLIZED 
	if (IsValid(SpawnedFertiliser) && SSComponent && SSComponent->IsActive() == false)
	{
		SSComponent->SetMeshesToStretch({ChickenMesh, SpawnedFertiliser->ItemComponent->Mesh});
		SSComponent->Enable();
	}
	else if (!IsValid(SpawnedFertiliser) && SSComponent && SSComponent->IsActive() == true)
	{
		SSComponent->Disable();
	}

	CheckForTooManyFertiliserBags();
}

UStaticMeshComponent* AFertiliserSpawner::GetMesh()
{
	return ItemComponent->Mesh;
}

void AFertiliserSpawner::Interact(APrototype2Character* _Player)
{
	if (!IsValid(SpawnedFertiliser))
		return;
	
	SpawnTimer = SpawnInterval;
	
	_Player->PickupItem(SpawnedFertiliser, EPickupActor::FertilizerActor);
	
	SpawnedFertiliser = nullptr;
}

void AFertiliserSpawner::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner,
                                               int _PlayerID)
{
	_InvokingWidget->SetHUDInteractText("Gather");

	if (!IsValid(SpawnedFertiliser))
		return;
	
	SpawnedFertiliser->ItemComponent->Mesh->SetRenderCustomDepth(true);
}

EInteractMode AFertiliserSpawner::IsInteractable(APrototype2PlayerState* _Player)
{
	if (bBagCountReachedMax)
		return INVALID;
	
	if (SpawnTimer <= 0.0f)
	{
		return INSTANT;
	}
	else
	{
		return INVALID;
	}
}

void AFertiliserSpawner::ClientInteract(APrototype2Character* _Player)
{
	SSComponent->Disable();
	
	if (!IsValid(SpawnedFertiliser))
		return;
	
	SpawnedFertiliser->Client_Pickup(_Player);
}

void AFertiliserSpawner::OnClientWalkAway(APrototype2Character* _Player)
{
	if (!IsValid(SpawnedFertiliser))
		return;
	
	SpawnedFertiliser->ItemComponent->Mesh->SetRenderCustomDepth(false);
}

bool AFertiliserSpawner::HasSpawnedFruit()
{
	if (IsValid(SpawnedFertiliser))
		return true;

	return false;
}

void AFertiliserSpawner::UpdateFertiliserMaterial()
{
	if (!IsValid(SpawnedFertiliser))
		return;
	
	if (bBagCountReachedMax)
	{
		SpawnedFertiliser->ItemComponent->Mesh->SetMaterial(0, GhostMaterial);
		SpawnedFertiliser->ItemComponent->Mesh->SetMaterial(1, GhostMaterial);
	}
	else
	{
		USeedData* FertiliserSeedData = SpawnedFertiliser->ServerData.SeedData;
		if (!IsValid(FertiliserSeedData))
			return;
	
		SpawnedFertiliser->ItemComponent->Mesh->SetMaterial(0, FertiliserSeedData->PacketMaterials[0]);
		SpawnedFertiliser->ItemComponent->Mesh->SetMaterial(1, FertiliserSeedData->PacketMaterials[1]);
	}
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
	if (!FertiliserPrefab)
		return;
	
	SpawnedFertiliser = GetWorld()->SpawnActor<AFertiliser>(FertiliserPrefab);
	SpawnedFertiliser->SetReplicates(true);
	SpawnedFertiliser->ItemComponent->Mesh->SetSimulatePhysics(false);
	SpawnedFertiliser->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnedFertiliser->ItemComponent->Mesh->SetIsReplicated(false);
	//SpawnedFertiliser->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Ignore);
	//FertiliserServerData.SpawnedFertiliser->ItemComponent->Multi_DisableCollisionAndAttach();

	TArray<USeedData*> RestrictedFertilizerDatas{};
	FHHExtraSettings& ExtraSettings = GameStateRef->ExtraSettings;
	if (ExtraSettings.bGoldFertiliser && FertiliserDatas.Num() > 0)
		RestrictedFertilizerDatas.Add(FertiliserDatas[0]);
	if (ExtraSettings.bConcreteFertiliser && FertiliserDatas.Num() > 1)
		RestrictedFertilizerDatas.Add(FertiliserDatas[1]);
	if (ExtraSettings.bPoisonFertiliser && FertiliserDatas.Num() > 2)
		RestrictedFertilizerDatas.Add(FertiliserDatas[2]);
	
	if (RestrictedFertilizerDatas.Num() > 0)
	{
		int8 ChanceOfGold = rand() % 2;
		if (ChanceOfGold == 0 && ExtraSettings.bGoldFertiliser)
		{
			USeedData* FertiliserData = RestrictedFertilizerDatas[0];
			SpawnedFertiliser->SetSeedData(FertiliserData, EPickupActor::FertilizerActor);
		}
		else
		{
			USeedData* FertiliserData = RestrictedFertilizerDatas[(rand() % (RestrictedFertilizerDatas.Num() - 1)) + 1];
			SpawnedFertiliser->SetSeedData(FertiliserData, EPickupActor::FertilizerActor);
		}
	}
	SpawnedFertiliser->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SpawnedFertiliser->SetActorRelativeLocation({SpawnXPosition, 0.0f, SpawnZPosition});
}

void AFertiliserSpawner::CheckForTooManyFertiliserBags()
{
	if (FertiliserDatas.Num() <= 0)
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
		UpdateFertiliserMaterial();
	}
	else
	{
		bBagCountReachedMax = false;
		UpdateFertiliserMaterial();
	}
}
