

#include "Seed.h"

#include "Prototype2/Characters/Prototype2Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/VFX/SquashAndStretch.h"

ASeed::ASeed()
{
	// make sure to rep
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InterfaceType = EInterfaceType::Default;

	ParachuteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Parachute"));
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParachuteMesh->SetupAttachment(RootComponent);
	ParachuteMesh->SetRelativeScale3D({2,2,2});
	ParachuteMesh->SetRelativeLocation(FVector::UpVector * 100);
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		DestroyVFX = PoofVFX.Class;
	}
}

void ASeed::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicatingMovement(true);
	
	if (HasAuthority())
	{
		SpawnPos = GetActorLocation();
		SpawnRotation = GetActorRotation();
		SpawnTime = GetWorld()->GetTimeSeconds();
		
		Lifetime = InitialLifetime;
		WiltDelayTimer = WiltDelay;
	}
}

void ASeed::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	HandleParachuteMovement();
	Wilt(_DeltaSeconds);
}

void ASeed::Destroyed()
{
	if (DestroyVFX && bShouldWilt)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(DestroyVFX, GetActorLocation(), FRotator{});
		SpawnedVFX->SetLifeSpan(5.0f);
	}
	
	Super::Destroyed();
}

void ASeed::Interact(APrototype2Character* _Player)
{
	if (!bHasLanded)
		return;
	
	ItemComponent->Interact(_Player, this);
	bShouldWilt = false;
	WiltDelayTimer = WiltDelay;
}

void ASeed::ClientInteract(APrototype2Character* _Player)
{
	if (!bHasLanded)
		return;

	_Player->EnableStencil(false);
	
	if (_Player->PlayerHUDRef)
		_Player->PlayerHUDRef->SetHUDInteractText("");
	
	ItemComponent->Mesh->SetRenderCustomDepth(false);
}

void ASeed::OnDisplayInteractText(class UWidget_PlayerHUD* InvokingWidget, class APrototype2Character* _Owner, int _PlayerID)
{
	if (!bHasLanded)
		return;
	
	if (!_Owner->HeldItem || _Owner->HeldItem != this)
	{
		InvokingWidget->SetHUDInteractText("Pick Up");

		_Owner->EnableStencil(true);
	}
}

EInteractMode ASeed::IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode)
{
	if (!bHasLanded)
		return INVALID;
	
	return INSTANT;
}

void ASeed::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASeed, bHasLanded);
	DOREPLIFETIME(ASeed, bShouldWilt);
	DOREPLIFETIME(ASeed, ParachuteSM);
}

void ASeed::SetParachuteMesh(UStaticMesh* _InMesh)
{
	ParachuteSM = _InMesh;
	OnRep_ParachuteMesh();
}

void ASeed::OnRep_ParachuteMesh()
{
	ParachuteMesh->SetStaticMesh(ParachuteSM);
}

void ASeed::OnRep_bHasLanded()
{
	if (bHasLanded)
	{
		if (DestroyVFX)
		{
			auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(DestroyVFX, ParachuteMesh->GetComponentLocation(), FRotator{});
			SpawnedVFX->SetLifeSpan(5.0f);
			SpawnedVFX->SetActorScale3D(FVector::One() * 3);
		}
		
		ParachuteMesh->SetVisibility(false);
	}
}

void ASeed::Wilt(float DeltaTime)
{
	if (!HasAuthority())
		return;
	
	if (!bShouldWilt)
		return;

	if (WiltDelayTimer > 0)
	{
		WiltDelayTimer -= DeltaTime;
	}
	else if (Lifetime > 0)
	{
		Lifetime -= DeltaTime;
		if (Lifetime <= 0)
		{
			Destroy();
		}
	}
}

void ASeed::Server_Drop()
{
	bShouldWilt = true;
}

void ASeed::HandleParachuteMovement()
{
	if (bHasLanded)
		return;
	
	if (HasAuthority())
	{
		if /* FALLING */ (GetActorLocation().Z >= (SpawnPos + (FVector::DownVector * DropDistance)).Z)
		{
			float ParachuteLifetime = GetWorld()->GetTimeSeconds() - SpawnTime;
			SetActorLocation(FMath::Lerp(SpawnPos, SpawnPos + (FVector::DownVector * DropDistance), ParachuteLifetime / FallTime), false, nullptr, ETeleportType::ResetPhysics);
		}
		else
		{
			ItemComponent->Mesh->SetSimulatePhysics(true);
			ItemComponent->Mesh->SetCenterOfMass({0.0f, 0.0f, -20.0});
			bHasLanded = true;
			bShouldWilt = true;

			OnRep_bHasLanded();
		}
	}
}
