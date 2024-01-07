

#include "Seed.h"

#include "Prototype2/Characters/Prototype2Character.h"
#include "Kismet/GameplayStatics.h"

ASeed::ASeed()
{
	// make sure to rep
	bReplicates = true;

	InterfaceType = EInterfaceType::Default;

	ParachuteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Parachute"));
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	//ParachuteMesh->SetIsReplicated(true);

	UE_LOG(LogTemp, Warning, TEXT("Map name: %s"), *levelName);
	if (levelName == "Level_Winter")
	{
		if (WinterParachute)
		{
			ParachuteMesh->SetStaticMesh(WinterParachute);
		}
	}
	else
	{
		if (NormalParachute)
		{
			ParachuteMesh->SetStaticMesh(NormalParachute);
		}
	}

	SpawnPos = GetActorLocation();
	SpawnRotation = GetActorRotation();
	SpawnTime = GetWorld()->GetTimeSeconds();
	ItemComponent->Mesh->SetSimulatePhysics(false);
}

void ASeed::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	
	HandleParachuteMovement();
}

void ASeed::Interact(APrototype2Character* _Player)
{
	ItemComponent->Interact(_Player, this);
	
	_Player->EnableStencil(false);
	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->SetHUDInteractText("");
	}
	ItemComponent->Mesh->SetRenderCustomDepth(false);
}

void ASeed::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);

	_Player->UpdateDecalDirection(true, false);
}

void ASeed::OnDisplayInteractText(class UWidget_PlayerHUD* InvokingWidget, class APrototype2Character* _Owner, int _PlayerID)
{
	if (!_Owner->HeldItem || _Owner->HeldItem != this)
	{
		InvokingWidget->SetHUDInteractText("Pick Up");

		_Owner->EnableStencil(true);
	}
}

bool ASeed::IsInteractable(APrototype2PlayerState* _Player)
{
	return true;
}

void ASeed::Multi_ToggleParachuteVisibility_Implementation(bool _Visible)
{

	if (HasAuthority())
		ItemComponent->Mesh->SetSimulatePhysics(!_Visible);
	
	ParachuteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ParachuteMesh->SetVisibility(_Visible);
}

void ASeed::HandleParachuteMovement()
{
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
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
		}
	}
}
