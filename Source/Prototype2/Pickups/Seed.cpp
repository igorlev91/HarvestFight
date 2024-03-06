

#include "Seed.h"

#include "Prototype2/Characters/Prototype2Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/VFX/SquashAndStretch.h"

ASeed::ASeed()
{
	// make sure to rep
	bReplicates = true;

	InterfaceType = EInterfaceType::Default;

	ParachuteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Parachute"));
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParachuteMesh->SetIsReplicated(true);

	PrimaryActorTick.bCanEverTick = true;

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
	
	ParachuteMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	ParachuteMesh->SetRelativeLocation(ParachuteMesh->GetRelativeLocation() + (FVector::UpVector * 100));
	ParachuteMesh->SetRelativeScale3D({2,2,2});
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParachuteMesh->SetSimulatePhysics(false);
	ParachuteMesh->SetVisibility(false);
	
	SpawnPos = GetActorLocation();
	SpawnRotation = GetActorRotation();
	SpawnTime = GetWorld()->GetTimeSeconds();
	ItemComponent->Mesh->SetSimulatePhysics(false);

	if (SeedData)
	{
		if (SeedData->BabyType == EPickupDataType::BeehiveData)
			ItemComponent->Mesh->SetWorldScale3D({2.0f,2.0f,2.0f});
	}

	Lifetime = InitialLifetime;
	WiltDelayTimer = WiltDelay;
}

void ASeed::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	if (bIsParachuteStaticMeshSet)
	{
		HandleParachuteMovement();
	}

	Wilt(_DeltaSeconds);
}

void ASeed::Interact(APrototype2Character* _Player)
{
	if (!bHasLanded)
		return;
	ItemComponent->Interact(_Player, this);
	
	Multi_OnInteract();
	bShouldWilt = false;
}

void ASeed::HoldInteract(APrototype2Character* _Player)
{
}

void ASeed::ClientInteract(APrototype2Character* _Player)
{
	if (!bHasLanded)
		return;
	
	IInteractInterface::ClientInteract(_Player);

	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);

	//SSComponent->Boing();

	WiltDelayTimer = WiltDelay;
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

bool ASeed::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!bHasLanded)
		return false;
	return true;
}

void ASeed::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASeed, ParachuteMesh);
	DOREPLIFETIME(ASeed, bIsParachuteStaticMeshSet);
	DOREPLIFETIME(ASeed, bHasLanded);
	DOREPLIFETIME(ASeed, bShouldWilt);
}

void ASeed::Multi_SetParachuteMesh_Implementation(UStaticMesh* _InMesh)
{

	ParachuteMesh->SetVisibility(true);
	ParachuteMesh->SetStaticMesh(_InMesh);
}

void ASeed::Server_SetParachuteMesh_Implementation(UStaticMesh* _InMesh)
{
	Multi_SetParachuteMesh(_InMesh);
}

void ASeed::Multi_ToggleParachuteVisibility_Implementation(bool _Visible)
{

	if (HasAuthority())
		ItemComponent->Mesh->SetSimulatePhysics(!_Visible);
	
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParachuteMesh->SetVisibility(_Visible);
}

void ASeed::SetParachuteMesh(UStaticMesh* _InMesh)
{
	if (_InMesh)
	{
		Multi_SetParachuteMesh(_InMesh);
		bIsParachuteStaticMeshSet = true;
	}
}

void ASeed::Wilt(float DeltaTime)
{
	if (!bShouldWilt)
		return;

	//WiltMaterial();

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

void ASeed::WiltMaterial()
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

void ASeed::Multi_OnInteract_Implementation()
{
	WiltDelayTimer = WiltDelay;
}

void ASeed::Server_Drop()
{
	bShouldWilt = true;
}

void ASeed::Multi_OnDestroy_Implementation()
{
	if (DestroyVFX)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(DestroyVFX, GetActorLocation(), FRotator{});
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}


void ASeed::HandleParachuteMovement()
{
	if (bHasLanded)
	{
		return;
	}
	
	if (HasAuthority())
	{
		if (GetActorLocation().Z - 1.0f >= (SpawnPos + (FVector::DownVector * DropDistance)).Z)
		{
			float XVariation = FMath::Sin(FMath::DegreesToRadians(GetWorld()->GetTimeSeconds()) * BobSpeed) * BobAmplitude;
			float ZVariation = FMath::Cos(FMath::DegreesToRadians(GetWorld()->GetTimeSeconds()) * BobSpeed ) * BobAmplitude;
			SetActorRotation({SpawnRotation.Pitch + XVariation, GetActorRotation().Yaw, SpawnRotation.Roll + ZVariation});
			float ParachuteLifetime = GetWorld()->GetTimeSeconds() - SpawnTime;
			SetActorLocation(FMath::Lerp(SpawnPos, SpawnPos + (FVector::DownVector * DropDistance), ParachuteLifetime / FallTime), false, nullptr, ETeleportType::ResetPhysics);
		}
		else
		{
			Multi_ToggleParachuteVisibility(false);
			bHasLanded = true;
			bShouldWilt = true;
			ItemComponent->Mesh->SetCenterOfMass({0.0f, 0.0f, -20.0});
		}
	}
}
