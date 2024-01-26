
#include "PreGameArena.h"

#include "Components/CapsuleComponent.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

APreGameArena::APreGameArena()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	bReplicates = true;
}

void APreGameArena::BeginPlay()
{
	Super::BeginPlay();



	if (!HasAuthority())
		return;
	
	GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
}

void APreGameArena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;
	
	if (!GameState)
		return;

	if (GameState->HasGameStarted())
		Destroy();
}

