

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
}

void ASeed::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	if (bIsParachuteStaticMeshSet)
	{
		HandleParachuteMovement();
	}
}

void ASeed::Interact(APrototype2Character* _Player)
{
	if (!bHasLanded)
		return;
	ItemComponent->Interact(_Player, this);
	

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
			float Lifetime = GetWorld()->GetTimeSeconds() - SpawnTime;
			SetActorLocation(FMath::Lerp(SpawnPos, SpawnPos + (FVector::DownVector * DropDistance), Lifetime / FallTime), false, nullptr, ETeleportType::ResetPhysics);
		}
		else
		{
			Multi_ToggleParachuteVisibility(false);
			bHasLanded = true;
			ItemComponent->Mesh->SetCenterOfMass({0.0f, 0.0f, -20.0});
		}
	}
}
