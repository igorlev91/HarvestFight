

#include "IcePatch.h"

AIcePatch::AIcePatch()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AIcePatch::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

