
#include "LaunchPad.h"

#include "Components/BoxComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"


// Sets default values
ALaunchPad::ALaunchPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LaunchPadMesh = CreateDefaultSubobject<UStaticMeshComponent>("LaunchPadMesh");
	SetRootComponent(LaunchPadMesh);

	LaunchCollider = CreateDefaultSubobject<UBoxComponent>("LaunchCollider");
	LaunchCollider->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaunchPad::LaunchPlayer(APrototype2Character* _Player)
{
	FVector LaunchVector = _Player->GetActorForwardVector();
	LaunchVector *= ForwardStrength;
	LaunchVector.Z = VerticalStrength;
	_Player->LaunchCharacter(LaunchVector, false, false);
}

