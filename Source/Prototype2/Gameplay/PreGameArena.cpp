
#include "PreGameArena.h"

#include "Components/CapsuleComponent.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

APreGameArena::APreGameArena()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	bReplicates = true;

	for(int i = 0; i < 9; i++)
	{
		FName ComponentName = FName("Spawn Location" + FString::FromInt(i));
		SpawnLocations[i] = CreateDefaultSubobject<USkeletalMeshComponent>(ComponentName);
		SpawnLocations[i]->SetupAttachment(RootComponent);
		SpawnLocations[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SpawnLocations[i]->SetVisibility(false);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FoundCowSkeletalMesh(TEXT("/Game/AlphaCharacters/CowV3/SM_Cow"));
	if (FoundCowSkeletalMesh.Object != NULL)
	{
		for(int i = 0; i < 9; i++)
		{
			SpawnLocations[i]->SetSkeletalMeshAsset(FoundCowSkeletalMesh.Object);
		}
	}
}

void APreGameArena::BeginPlay()
{
	Super::BeginPlay();
	
	TeamsMaterial = Mesh->CreateDynamicMaterialInstance(0);

	Mesh->SetIsReplicated(false);
	SetReplicateMovement(true);
	
	if (!HasAuthority())
		return;
	
	FRotator Rotation{0.0f, 45.0f, 0.0f};
	SetActorRotation(Rotation);
	Mesh->SetWorldRotation(Rotation);
	
	GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	
	TArray<AActor*> Players{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototype2Character::StaticClass(), Players);

	for (auto Player : Players)
	{
		Player->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 100));
	}
}

void APreGameArena::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APrototype2Gamestate* SomeGamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (SomeGamestate->bTeams)
		{
			if (AssignedTeam == 0)
				TeamsMaterial->SetVectorParameterValue(FName("ColourTint"), ColourData->PureColours[(uint8)SomeGamestate->TeamOneColour]);
			else
				TeamsMaterial->SetVectorParameterValue(FName("ColourTint"), ColourData->PureColours[(uint8)SomeGamestate->TeamTwoColour]);
		
			Mesh->SetMaterial(0, TeamsMaterial);
		}
	}
	
	if (!HasAuthority())
		return;
	
	if (!GameState)
		return;

	if (GameState->HasGameStarted())
	{
		APrototype2GameMode* GamemodeRef = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (IsValid(GamemodeRef))
		{
			GamemodeRef->PreGameArenas.Remove(this);
			Destroy(true);
		}
	}

}

void APreGameArena::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APreGameArena, AssignedTeam);
}

FVector APreGameArena::GetNextSpawnLocation()
{
	auto SpawnLocation = SpawnLocations[FMath::Clamp(SpawnLocationIndex, 0, 8)]->GetComponentLocation();
	SpawnLocationIndex++;
	return SpawnLocation;
}

void APreGameArena::Multi_SetArenaColour_Implementation()
{

	Mesh->SetMaterial(0, TeamsMaterial);
}

