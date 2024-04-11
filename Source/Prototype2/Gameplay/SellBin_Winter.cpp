#include "SellBin_Winter.h"

#include "Prototype2/Characters/Prototype2Character.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ASellBin_Winter::ASellBin_Winter()
	: ASellBin()
{
	bReplicates = true;

	IcePlane = CreateDefaultSubobject<UStaticMeshComponent>("Ice Plane");
	IcePlane->SetWorldLocation({-104.559325,-72.190911,-13.473242});
	IcePlane->SetupAttachment(RootComponent);
	
	IceBoundary = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Boundary");
	IceBoundary->SetRelativeScale3D({1,1,8});
	IceBoundary->SetWorldLocation({-104.559325,-72.190911,300});
	IceBoundary->SetupAttachment(RootComponent);

	InterfaceType = EInterfaceType::SellBin;
	
}

void ASellBin_Winter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	
	IcePlane->SetCollisionProfileName("NoCollision");
	IcePlane->SetCollisionObjectType(ECC_WorldStatic);
	IcePlane->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IcePlane->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	IcePlane->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	IcePlane->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	IcePlane->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Block);
	if (IsValid(RootComponent))
		IcePlane->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IcePlane->SetMobility(EComponentMobility::Static);
	IcePlane->SetIsReplicated(false);

	IceBoundary->SetHiddenInGame(true);
	IceBoundary->SetCollisionProfileName("NoCollision");
	IceBoundary->SetCollisionObjectType(ECC_WorldStatic);
	IceBoundary->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IceBoundary->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	IceBoundary->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	IceBoundary->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	IceBoundary->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	IceBoundary->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	if (IsValid(RootComponent))
		IceBoundary->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IceBoundary->SetMobility(EComponentMobility::Static);
	IceBoundary->SetIsReplicated(false);

	ItemComponent->Mesh->SetCollisionProfileName("Vehicle");
	ItemComponent->Mesh->SetMassOverrideInKg(NAME_None, 100);
	
	//
	if (!HasAuthority())
		return;

	ItemComponent->Mesh->SetSimulatePhysics(true);
	ItemComponent->Mesh->OnComponentBeginOverlap.AddDynamic(this, &ASellBin_Winter::OnCollision);
}

void ASellBin_Winter::GetHit(float _AttackCharge, float _MaxAttackCharge, FVector _AttackerLocation)
{
	auto AttackLoc = FVector3d{_AttackerLocation.X, _AttackerLocation.Y, 0.0};
	auto MyLoc = FVector3d{GetActorLocation().X, GetActorLocation().Y, 0.0};
	auto ForceMultiplier = FMath::Lerp(MinForceFromPlayerHit, MaxForceFromPlayerHit, _AttackCharge / _MaxAttackCharge);
	ForceMultiplier = FMath::Clamp(ForceMultiplier, MinForceFromPlayerHit, MaxForceFromPlayerHit);
	FVector Force = ((MyLoc - AttackLoc).GetSafeNormal()) * ForceMultiplier;
	ItemComponent->Mesh->AddImpulse(Force);

	//UE_LOG(LogTemp, Warning, TEXT("Player hit Box with %s multiplier"), *FString::FromInt(forceMultiplier));
}

void ASellBin_Winter::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (auto Player = Cast<APrototype2Character>(OtherActor))
	{
		if (Player->IsSprinting())
		{
			//
			auto AttackLoc = FVector3d{OtherActor->GetActorLocation().X, OtherActor->GetActorLocation().Y, 0.0};
			auto MyLoc = FVector3d{GetActorLocation().X, GetActorLocation().Y, 0.0};
			auto Force = (AttackLoc - MyLoc).GetSafeNormal() * MinForceFromPlayerHit;
			ItemComponent->Mesh->AddImpulseAtLocation({0,0,ImpactLocationZ}, Force);

			//UE_LOG(LogTemp, Warning, TEXT("Player Touched Box"));
		}
	}
}

