// Fill out your copyright notice in the Description page of Project Settings.


#include "SellBin_Winter.h"

#include "Prototype2Character.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ASellBin_Winter::ASellBin_Winter()
{
	Collision = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent({115,115,175});

	IcePlane = CreateDefaultSubobject<UStaticMeshComponent>("Ice Plane");
	IcePlane->SetupAttachment(RootComponent);
	
	IceBoundary = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Boundary");
	IceBoundary->SetupAttachment(RootComponent);
}

void ASellBin_Winter::BeginPlay()
{
	Super::BeginPlay();

	IcePlane->SetupAttachment(RootComponent);
	IcePlane->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	IceBoundary->SetupAttachment(RootComponent);
	IceBoundary->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	Collision->SetupAttachment(RootComponent);
	Collision->SetBoxExtent({115,115,175});
	
	ItemComponent->Mesh->SetSimulatePhysics(true);
	ItemComponent->Mesh->SetMassOverrideInKg(NAME_None, 100.0f);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASellBin_Winter::OnCollision);
}

void ASellBin_Winter::GetHit(float AttackCharge, float _maxAttackCharge, FVector AttackerLocation)
{
	auto attackLoc = FVector3d{AttackerLocation.X, AttackerLocation.Y, 0.0};
	auto myLoc = FVector3d{GetActorLocation().X, GetActorLocation().Y, 0.0};
	auto forceMultiplier = FMath::Lerp(MinForceFromPlayerHit, MaxForceFromPlayerHit, AttackCharge / _maxAttackCharge);
	forceMultiplier = FMath::Clamp(forceMultiplier, MinForceFromPlayerHit, MaxForceFromPlayerHit);
	FVector force = ((attackLoc - myLoc).GetSafeNormal()) * forceMultiplier;
	ItemComponent->Mesh->AddImpulseAtLocation({0,0,ImpactLocationZ}, force);

	UE_LOG(LogTemp, Warning, TEXT("Player hit Box with %s multiplier"), *FString::FromInt(forceMultiplier));
}

void ASellBin_Winter::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (auto* player = Cast<APrototype2Character>(OtherActor))
	{
		if (player->IsSprinting())
		{
			auto attackLoc = FVector3d{OtherActor->GetActorLocation().X, OtherActor->GetActorLocation().Y, 0.0};
			auto myLoc = FVector3d{GetActorLocation().X, GetActorLocation().Y, 0.0};
			auto force = (attackLoc - myLoc).GetSafeNormal() * MinForceFromPlayerHit;
			ItemComponent->Mesh->AddImpulseAtLocation({0,0,ImpactLocationZ}, force);
			UE_LOG(LogTemp, Warning, TEXT("Player Touched Box"));
		}
	}
}

