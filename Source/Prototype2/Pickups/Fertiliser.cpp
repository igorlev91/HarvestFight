

#include "Fertiliser.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/Spawning/FertiliserSpawner.h"

AFertiliser::AFertiliser()
{
	bReplicates = true;

	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		DestroyVFX = PoofVFX.Class;
	}
}

void AFertiliser::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Wilt(DeltaSeconds);
}

void AFertiliser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFertiliser, bShouldWilt);
}

void AFertiliser::BeginPlay()
{
	Super::BeginPlay();
	ServerData.PickupActor = EPickupActor::FertilizerActor;
	SetReplicatingMovement(true);

	if (ServerData.SeedData)
		SetActorScale3D(ServerData.SeedData->BabyScale);

	ItemComponent->SetStencilEnabled(false);

	Lifetime = InitialLifetime;
	WiltDelayTimer = WiltDelay;

	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFertiliser::Interact(APrototype2Character* _Player)
{
	ItemComponent->Interact(_Player, this);

	Multi_OnInteract();
	bShouldWilt = false;
}

void AFertiliser::ClientInteract(APrototype2Character* _Player)
{
	_Player->EnableStencil(false);
	
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	WiltDelayTimer = WiltDelay;
}

void AFertiliser::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	if (!_Owner->HeldItem || _Owner->HeldItem != this)
	{
		_InvokingWidget->SetHUDInteractText("Pick Up");

		_Owner->EnableStencil(true);
	}
}

EInteractMode AFertiliser::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return INVALID;

	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Casted = Cast<APrototype2Character>(Character))
			{
				if (!Casted->HeldItem || Casted->HeldItem != this)
				{
					return INSTANT;
				}
			}
		}
	}
	
	return INVALID;
}

void AFertiliser::Wilt(float DeltaTime)
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

void AFertiliser::WiltMaterial()
{
	if (Materials.Num() <= 0)
	{
		for(int i = 0; i < ItemComponent->Mesh->GetNumMaterials(); i++)
		{
			auto MaterialInstance = ItemComponent->Mesh->CreateDynamicMaterialInstance(i, ItemComponent->Mesh->GetMaterial(i));
			Materials.Add(MaterialInstance);
			ItemComponent->Mesh->SetMaterial(i , MaterialInstance);
		}
	}
	
	for(int i = 0; i < Materials.Num(); i++)
	{
		Materials[i]->SetScalarParameterValue(FName("GrayScaleTint"), FMath::Lerp<float>(1.0f, 0.0f, Lifetime / InitialLifetime));

		//UE_LOG(LogTemp, Warning, TEXT("wilted plant material"));
	}
}

void AFertiliser::Multi_OnInteract_Implementation()
{
	WiltDelayTimer = WiltDelay;
}

void AFertiliser::Server_Drop()
{
	bShouldWilt = true;
}

void AFertiliser::Multi_OnDestroy_Implementation()
{
	if (DestroyVFX)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(DestroyVFX, GetActorLocation(), FRotator{});
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}
