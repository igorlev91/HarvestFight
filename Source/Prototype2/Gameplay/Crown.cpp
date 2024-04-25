

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
	RootComponent = RootTransformComponent;
	RootTransformComponent->SetIsReplicated(true);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetIsReplicated(true);
	BobTransformComponent = CreateDefaultSubobject<UBobTransformComponent>(TEXT("Bob Component"));
}

void ACrown::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	Mesh->SetVisibility(false);

	if (!HasAuthority())
		return;

	BobTransformComponent->SetEnabled(false);
}

void ACrown::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrown, Mesh);
	DOREPLIFETIME(ACrown, RootTransformComponent);
}

void ACrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRelativeRotation({-70.0f, -20.0f, 0.0f});
	
	if (!HasAuthority())
		return;

	SetActorRelativeLocation({6.000000,-3.000000,2.330000});
	SetActorRelativeScale3D({0.1f, 0.1f, 0.1f});

	if (!GameStateRef)
		GameStateRef = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));

	if (UpdateDelayTimer <= 0)
		AttachToCurrentWinner();
	else
		UpdateDelayTimer -= DeltaTime;
}

void ACrown::AttachToCurrentWinner()
{
	if (!GameStateRef)
		return;
	
	UpdateDelayTimer = UpdateDelay;

	ACharacter* WinningPlayerCharacter{nullptr};
	APrototype2PlayerState* WinningPlayer {nullptr};
	
	for(int i = 0; i < GameStateRef->PlayerArray.Num(); i++)
	{
		APlayerController* PlayerController = GameStateRef->PlayerArray[i].Get()->GetPlayerController();
		if (!PlayerController)
			continue;

		ACharacter* Character = PlayerController->GetCharacter();
		if (!Character)
			continue;

		//if (i == 0)
		//{
		//	WinningPlayerCharacter = Character;
		//	WinningPlayer = Cast<APrototype2PlayerState>(GameStateRef->PlayerArray[i]);
		//	WinnerID = i;
		//	continue;
		//}
		//else if (auto CastedPlayerState = Cast<APrototype2PlayerState>(GameStateRef->PlayerArray[i]))
		//{
		//	if (CastedPlayerState->Coins > WinningPlayer->Coins)
		//	{
		//		WinningPlayerCharacter = Character;
		//		WinningPlayer = Cast<APrototype2PlayerState>(GameStateRef->PlayerArray[i].Get());
		//		WinnerID = i;
		//	}
		//}
		
		if (auto Player = Cast<APrototype2PlayerState>(GameStateRef->PlayerArray[i]))
		{
			if (Player->Coins > GameStateRef->GetWinningScore())
			{
				WinningPlayerCharacter = Character;
				WinningPlayer = Player;
				GameStateRef->SetPlayerWinner(i);
				GameStateRef->SetWinningScore(Player->Coins);
			}
		}
	}

	if (WinningPlayer)
	{
		if (WinningPlayer->Coins <= 0)
		{
			Mesh->SetVisibility(false);
		}
		else if (!IsAttachedTo(WinningPlayerCharacter))
		{
			UE_LOG(LogTemp, Warning, TEXT("Attach Crown To Winning Player") );
			Mesh->SetVisibility(true);
			AttachToComponent(WinningPlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Base-HumanHead"));
			SetActorRelativeLocation({6.000000,-3.000000,2.330000});
			SetActorRelativeScale3D({0.1f, 0.1f, 0.1f});

			Server_OnWinnerTakesTheCrown(WinningPlayer->Player_ID);
		}
	}
}

void ACrown::Server_OnWinnerTakesTheCrown_Implementation(int32 _PlayerID)
{
	Multi_OnWinnerTakesTheCrown(_PlayerID);
}

void ACrown::Multi_OnWinnerTakesTheCrown_Implementation(int32 _PlayerID)
{
	OnWinnerTakesTheCrownDelegate.Broadcast(_PlayerID);
}

void ACrown::Multi_AttachToCurrentWinner_Implementation(USkeletalMeshComponent* _WinterMesh)
{
	
}

UStaticMeshComponent* ACrown::GetMesh()
{
	return Mesh;
}

