

#include "HoneyGoopPatch.h"


// Sets default values
AHoneyGoopPatch::AHoneyGoopPatch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AHoneyGoopPatch::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

