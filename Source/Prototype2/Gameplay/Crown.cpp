

#include "Crown.h"

#include "BobTransformComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

ACrown::ACrown()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	RootTransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootTransformComponent"));
	SetRootComponent(RootTransformComponent);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void ACrown::BeginPlay()
{
	Super::BeginPlay();
	
	GameStateRef = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));

	if (AttachedPlayer == nullptr)
		Mesh->SetVisibility(false);
}

void ACrown::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrown, AttachedPlayer);
}

void ACrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!HasAuthority())
		return;

	AttachToCurrentWinner();
}

void ACrown::AttachToCurrentWinner()
{
	if (IsValid(GameStateRef) == false)
		return;

	APrototype2Character* WinningPlayer = GameStateRef->GetWinningCharacter();
	if (WinningPlayer != AttachedPlayer)
	{
		AttachedPlayer = WinningPlayer;

		if (IsValid(WinningPlayer))
		{
			GameStateRef->SetPlayerWinner(WinningPlayer->GetPlayerID());
			
			APrototype2PlayerState* WinningPlayerState = WinningPlayer->GetPlayerState<APrototype2PlayerState>();
			if (IsValid(WinningPlayerState))
			{
				GameStateRef->SetWinningScore(WinningPlayerState->Coins);
			}
		}
	
		OnRep_AttachedPlayer();
	}
}

void ACrown::OnRep_AttachedPlayer()
{
	if (IsValid(AttachedPlayer) == false)
	{
		Mesh->SetVisibility(false);
		return;
	}

	Mesh->SetVisibility(true);
	AttachToComponent(AttachedPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("S_Crown"));
	//OnWinnerTakesTheCrownDelegate.Broadcast(AttachedPlayer->GetPlayerID());
	OnWinnerTakesTheCrownDelegate.Broadcast(AttachedPlayer->PlayerStateRef->PlayerName);
	
}
