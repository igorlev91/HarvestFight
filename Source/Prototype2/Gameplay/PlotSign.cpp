
#include "PlotSign.h"

#include "RadialPlot.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Widgets/Widget_PlayerHUD.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/VFX/SquashAndStretch.h"

APlotSign::APlotSign()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void APlotSign::BeginPlay()
{
	Super::BeginPlay();

	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	SSComponent->Enable();
	
	if (HasAuthority())
		SSComponent->SetMeshToStretch({ItemComponent->Mesh});
}

void APlotSign::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlotSign, bHasBeenClaimed);
}

void APlotSign::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPlayerClaimingPlot)
	{
		if (CurrentPlayerClaimingPlot->bIsHoldingInteract == false)
		{
			// Talk to HUD through delegates
			CurrentPlayerClaimingPlot->Client_OnStoppedClaimingPlot();
			
			UKismetSystemLibrary::PrintString(GetWorld(), "STOPPED CLAIMING PLOT!");
			CurrentPlayerClaimingPlot = nullptr;
		}
	}
}

UStaticMeshComponent* APlotSign::GetMesh()
{
	return ItemComponent->Mesh;
}

void APlotSign::Interact(APrototype2Character* _Player)
{
	if (bHasBeenClaimed)
		return;

	if (CurrentPlayerClaimingPlot)
		return;
	
	// anything that needs to happen once
	_Player->bIsHoldingInteract = true;
	CurrentPlayerClaimingPlot = _Player;
	HoldInteractTimer = 0.0f;
}

void APlotSign::HoldInteract(APrototype2Character* _Player)
{
	if (!_Player->bIsHoldingInteract)
	return;
	
	HoldInteractTimer += GetWorld()->DeltaTimeSeconds;

	// Talk to HUD through delegates
	_Player->Client_OnClaimingPlot(HoldInteractTimer, HoldInteractTotalDuration);
	
	if (HoldInteractTimer >= HoldInteractTotalDuration)
	{
		ClaimPlot(_Player);
		_Player->bIsHoldingInteract = false;
	}
}

void APlotSign::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	if (!_Owner)
		return;
	
	if (bHasBeenClaimed)
		return;

	_InvokingWidget->SetHUDInteractText("Claim");
	ItemComponent->Mesh->SetRenderCustomDepth(true);
}

bool APlotSign::IsInteractable(APrototype2PlayerState* _Player)
{
	ACharacter* InstigatorCharacter = _Player->GetPlayerController()->GetCharacter();
	if (!InstigatorCharacter)
		return false;

	APrototype2Character* CastedInstigatorCharacter = Cast<APrototype2Character>(InstigatorCharacter);
	if (!CastedInstigatorCharacter)
		return false;
	
	if (bHasBeenClaimed || CastedInstigatorCharacter->HasClaimedPlot())
		return false;
	
	return true;
}

void APlotSign::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);
}

void APlotSign::ClaimPlot(APrototype2Character* _Player)
{
	AActor* Parent = GetAttachParentActor();
	if (!Parent)
		return;

	ARadialPlot* RadialPlot = Cast<ARadialPlot>(Parent);
	if (!RadialPlot)
		return;

	bHasBeenClaimed = true;
	RadialPlot->SpawnGrowSpots(_Player->GetPlayerState<APrototype2PlayerState>()->Details.Colour);
	_Player->SetClaimedPlot(RadialPlot);
}

