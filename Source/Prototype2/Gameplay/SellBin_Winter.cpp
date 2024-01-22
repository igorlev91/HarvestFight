#include "SellBin_Winter.h"

#include "Prototype2/Characters/Prototype2Character.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ASellBin_Winter::ASellBin_Winter()
{
	bReplicates = true;
	Collision = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent({115,115,175});
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	IcePlane = CreateDefaultSubobject<UStaticMeshComponent>("Ice Plane");
	IcePlane->SetWorldLocation({-104.559325,-72.190911,-13.473242});
	//IcePlane->SetPhysMaterialOverride()
	
	IceBoundary = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Boundary");
	IceBoundary->SetWorldLocation({-104.559325,-72.190911,300});
	IceBoundary->SetCollisionProfileName(FName("Vehicle"));
	IceBoundary->SetRelativeScale3D({1,1,8});

	InterfaceType = EInterfaceType::SellBin;
	
}

void ASellBin_Winter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	IcePlane->SetupAttachment(ItemComponent->Mesh);
	IcePlane->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IcePlane->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	IcePlane->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Block);
	
	IceBoundary->SetupAttachment(ItemComponent->Mesh);
	//IceBoundary->SetHiddenInGame(true);
	IceBoundary->SetCollisionProfileName("NoCollision");
	IceBoundary->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	IceBoundary->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	IceBoundary->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	IceBoundary->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	ItemComponent->Mesh->SetMassOverrideInKg(NAME_None, 200.0f);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);


	//
	if (!HasAuthority())
		return;

	ItemComponent->Mesh->SetSimulatePhysics(true);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASellBin_Winter::OnCollision);
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

void ASellBin_Winter::SetShippingBinPosition_Networked(FVector _Pos)
{
	IcePlane->SetupAttachment(RootComponent);
	IcePlane->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IcePlane->SetRelativeLocation({});

	IceBoundary->SetupAttachment(RootComponent);
	IceBoundary->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IceBoundary->SetRelativeLocation({});

	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	ItemComponent->Mesh->SetSimulatePhysics(true);
	ItemComponent->Mesh->SetMassOverrideInKg(NAME_None, 100.0f);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	ItemComponent->Mesh->SetConstraintMode(EDOFMode::XYPlane);
	Collision->SetConstraintMode(EDOFMode::XYPlane);
}

void ASellBin_Winter::Server_DetachComponents_Implementation(FVector _Pos)
{
	
}

void ASellBin_Winter::Multi_DetachComponents_Implementation(FVector _Pos)
{
	SetActorLocation(_Pos);

	IcePlane->SetupAttachment(RootComponent);
	IcePlane->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IcePlane->SetRelativeLocation({});
	IcePlane->SetCollisionResponseToChannel(ECC_EngineTraceChannel2, ECollisionResponse::ECR_Block);

	IceBoundary->SetupAttachment(RootComponent);
	IceBoundary->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	IceBoundary->SetRelativeLocation({});
	
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent({115,115,175});
	
	ItemComponent->Mesh->SetSimulatePhysics(true);
	ItemComponent->Mesh->SetMassOverrideInKg(NAME_None, 100.0f);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	ItemComponent->Mesh->SetConstraintMode(EDOFMode::XYPlane);
	Collision->SetConstraintMode(EDOFMode::XYPlane);
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

