


#include "Plant.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/DataAssets/SeedData.h"

APlant::APlant()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	InterfaceType = EInterfaceType::Default;

}

void APlant::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicatingMovement(true);

	Lifetime = InitialLifetime;
	WiltDelayTimer = WiltDelay;
}


void APlant::Interact(APrototype2Character* _Player)
{
	ItemComponent->Interact(_Player, this);

	Multi_OnInteract();
	bShouldWilt = false;
}

void APlant::HoldInteract(APrototype2Character* _Player)
{
}

void APlant::ClientInteract(APrototype2Character* _Player)
{
	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);
	
	//SSComponent->Boing();

	WiltDelayTimer = WiltDelay;
}

void APlant::OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID)
{
	if (!_Owner->HeldItem || _Owner->HeldItem != this)
	{
		_InvokingWidget->SetHUDInteractText("Pick Up");

		_Owner->EnableStencil(true);
	}
}

bool APlant::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!bGrown)
	{
		return false;
	}
	
	if (!_Player)
		return false;

	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Casted = Cast<APrototype2Character>(Character))
			{
				if (!Casted->HeldItem || Casted->HeldItem != this)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

void APlant::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Wilt(DeltaSeconds);
}

void APlant::Server_Drop()
{
	bShouldWilt = true;
}

void APlant::Client_Drop()
{
}

void APlant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlant, bShouldWilt);
	DOREPLIFETIME(APlant, bPoisoned);
	DOREPLIFETIME(APlant, bGrown);
}

void APlant::Wilt(float DeltaTime)
{
	if (!bShouldWilt)
		return;

	WiltMaterial();

	if (WiltDelayTimer > 0)
	{
		WiltDelayTimer -= DeltaTime;
	}
	else if (Lifetime > 0)
	{
		Lifetime -= DeltaTime;
		if (Lifetime <= 0)
		{
			if (HasAuthority())
			{
				Multi_OnDestroy();
				Destroy();
			}
		}
	}
}

void APlant::WiltMaterial()
{
	if (PlantMats.Num() <= 0)
	{
		for(int i = 0; i < ItemComponent->Mesh->GetNumMaterials(); i++)
		{
			auto MaterialInstance = ItemComponent->Mesh->CreateDynamicMaterialInstance(i, ItemComponent->Mesh->GetMaterial(i));
			PlantMats.Add(MaterialInstance);
			ItemComponent->Mesh->SetMaterial(i , MaterialInstance);
		}
	}
	
	for(int i = 0; i < PlantMats.Num(); i++)
	{
		PlantMats[i]->SetScalarParameterValue(FName("GrayScaleTint"), FMath::Lerp<float>(1.0f, 0.0f, Lifetime / InitialLifetime));

		//UE_LOG(LogTemp, Warning, TEXT("wilted plant material"));
	}
}

void APlant::Multi_OnInteract_Implementation()
{
	WiltDelayTimer = WiltDelay;
}

void APlant::Server_Destroy_Implementation()
{
	Destroy();
}

void APlant::Multi_ScalePlant()
{
	ItemComponent->Mesh->SetWorldScale3D(ServerData.SeedData->BabyScale);
}

void APlant::Multi_OnDestroy_Implementation()
{
	if (DestroyVFX)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(DestroyVFX, GetActorLocation(), FRotator{});
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}
