#include "SellBin.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Prototype2/Pickups/Plant.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Components/TextBlock.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/PlayerState.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/DataAssets/SellBinData.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/Widgets/Widget_SellCropUI.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Gameplay/RandomEventManager.h"

ASellBin::ASellBin()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	
	// Sell UI
	//SellAmountWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SellAmountWidgetComponent"));
	//SellAmountWidgetComponent->SetupAttachment(RootComponent);
	//SellAmountWidgetComponent->SetIsReplicated(false);
	//SellAmountWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	InterfaceType = EInterfaceType::SellBin;

	InteractSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	InteractSystem->SetupAttachment(RootComponent);
	InteractSystem->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CoinsVFX(TEXT("/Game/VFX/AlphaVFX/NiagaraSystems/NS_SellCoins"));
	if (CoinsVFX.Object->IsValid())
	{
		InteractSystem->SetAsset(CoinsVFX.Object);
	}

	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		PoofSystem = PoofVFX.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> FoundSellCue(TEXT("/Game/SFX/CUE_Sell"));
	if (FoundSellCue.Object != nullptr)
	{
		SellCue = FoundSellCue.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> FoundPumpkinCue(TEXT("/Game/SFX/MostValueCrop/Pumpkin_Cue"));
	if (FoundPumpkinCue.Object != nullptr)
	{
		EnemySellCue = FoundPumpkinCue.Object;
	}

	ThrowToSellCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ThrowToSellCollider"));
	ThrowToSellCollider->SetupAttachment(RootComponent);
}

void ASellBin::BeginPlay()
{
	Super::BeginPlay();

	InterfaceType = EInterfaceType::SellBin;
	
	ItemComponent->Mesh->SetMobility(EComponentMobility::Movable);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	
	if (SellBinData)
	{
		ItemComponent->Mesh->SetWorldScale3D(SellBinData->DesiredScale);
	}

	//SSComponent->SetMeshToStretch(ItemComponent->Mesh);

	if (HasAuthority())
	{
		ItemComponent->Mesh->SetNotifyRigidBodyCollision(true);
		ItemComponent->Mesh->OnComponentHit.AddDynamic(this, &ASellBin::OnPlayerTouchSellBin);
		ItemComponent->Mesh->SetIsReplicated(true);
		
		ThrowToSellCollider->OnComponentBeginOverlap.AddDynamic(this, &ASellBin::SellOnThrown);
	}
}

void ASellBin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	
	//MoveUIComponent(DeltaTime);
}

EInteractMode ASellBin::IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode)
{
	return INVALID;
}

void ASellBin::ClientInteract(APrototype2Character* _Player)
{
	// Force Bump Into SellBin
}

void ASellBin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASellBin, ServerSellData);
}

void ASellBin::OnPlayerTouchSellBin(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor) == false)
		return;

	APrototype2Character* PlayerCharacter = Cast<APrototype2Character>(OtherActor);
	if (IsValid(PlayerCharacter) == false)
		return;

	if (IsValid(PlayerCharacter->HeldItem) == false)
		return;

	auto Plant = Cast<APlant>(PlayerCharacter->HeldItem);
	if (IsValid(Plant) == false)
		return;
	
	int32 PlantSellValue = (Plant->ServerData.SeedData->BabyStarValue * Plant->ServerData.SeedData->PlantData->Multiplier * (Plant->NumberOfNearbyFlowers + 1));
	int32 IncreaseAmount = Plant->ItemComponent->bGold ? PlantSellValue * Plant->ServerData.SeedData->GoldMultiplier : PlantSellValue;
	
	PlayerCharacter->PlayerStateRef->AddCoins(IncreaseAmount);
	PlayerCharacter->HeldItem->Destroy();
	PlayerCharacter->HeldItem = nullptr;
	PlayerCharacter->OnRep_HeldItem();
	
	FServerSellData NewServerSellData{};
	NewServerSellData.StarValue = Plant->ServerData.SeedData->BabyStarValue;
	NewServerSellData.SellValue = IncreaseAmount;
	NewServerSellData.LastPlayerToSell = PlayerCharacter;
	NewServerSellData.LastPlayerStateToSell = PlayerCharacter->PlayerStateRef;
	NewServerSellData.TimeOfSell = GetWorld()->GetTimeSeconds();
	ServerSellData = NewServerSellData;
	
	OnRep_ItemSold();
}

void ASellBin::SellOnThrown(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (IsValid(OtherActor) == false)
		return;

	APlant* ThrownPlant = Cast<APlant>(OtherActor);
	if (IsValid(ThrownPlant) == false)
		return;
	
	auto PlayerWhoThrewItem = ThrownPlant->ItemComponent->PlayerWhoThrewItem;
	if (IsValid(PlayerWhoThrewItem) == false)
		return;

	int32 PlantSellValue = (ThrownPlant->ServerData.SeedData->BabyStarValue * ThrownPlant->ServerData.SeedData->PlantData->Multiplier * (ThrownPlant->NumberOfNearbyFlowers + 1));
	int32 IncreaseAmount = ThrownPlant->ItemComponent->bGold ? PlantSellValue * ThrownPlant->ServerData.SeedData->GoldMultiplier : PlantSellValue;
	
	PlayerWhoThrewItem->PlayerStateRef->AddCoins(IncreaseAmount);
	ThrownPlant->Destroy();
	
	FServerSellData NewServerSellData{};
	NewServerSellData.StarValue = ThrownPlant->ServerData.SeedData->BabyStarValue;
	NewServerSellData.SellValue = IncreaseAmount;
	NewServerSellData.LastPlayerToSell = PlayerWhoThrewItem;
	NewServerSellData.LastPlayerStateToSell = PlayerWhoThrewItem->PlayerStateRef;
	NewServerSellData.TimeOfSell = GetWorld()->GetTimeSeconds();
	ServerSellData = NewServerSellData;

	OnRep_ItemSold();
}

void ASellBin::OnRep_ItemSold()
{
	auto SellValue = ServerSellData.SellValue;
	auto StarValue = ServerSellData.StarValue;
	auto LastPlayerToSell = ServerSellData.LastPlayerToSell;
	auto LastPlayerStateToSell = ServerSellData.LastPlayerStateToSell;
	
	if (SellValue <= 0)
		return;

	if (IsValid(LastPlayerToSell) == false)
		return;

	if (IsValid(LastPlayerStateToSell) == false)
		return;

	auto LocalPlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(LocalPlayerController) == false)
		return;

	if (LocalPlayerController->IsLocalController() == false)
		return;

	if (GetWorld()->GetRealTimeSeconds() <= 5.0f)
		return;

	auto LocalPlayerState = LocalPlayerController->GetPlayerState<APrototype2PlayerState>();
	if (IsValid(LocalPlayerState) == false)
		return;
	
	// Sell SFX
	if (LocalPlayerState->Details.Colour == LastPlayerStateToSell->Details.Colour)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SellCue, GetActorLocation());
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EnemySellCue, GetActorLocation());

	// Coin VFX
	InteractSystem->SetVectorParameter(FName("Coin Colour"), LastPlayerStateToSell->Details.PureToneColour);
	InteractSystem->SetIntParameter(FName("CoinCount"), SellValue * StarValue);
	InteractSystem->Activate(true);

	// Poof VFX
	if (PoofSystem)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, InteractSystem->GetComponentLocation(), FRotator{});
		SpawnedVFX->SetActorScale3D(FVector::One() * 1.5f);
		SpawnedVFX->SetLifeSpan(5.0f);
	}
	
	APrototype2Gamestate* GamestateCast = GetWorld()->GetGameState<APrototype2Gamestate>();
	if (IsValid(GamestateCast) == false)
		return;

	if (GamestateCast->Server_Players.Contains(LastPlayerStateToSell))
	{
		OnItemSoldDelegate.Broadcast(GamestateCast->Server_Players.Find(LastPlayerStateToSell), SellValue);
	}
}

void ASellBin::Interact(APrototype2Character* _Player)
{
	// Force Bump Into SellBin
}

void ASellBin::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	if(auto HeldItem = _Owner->HeldItem)
	{
		if (HeldItem->ServerData.PickupActor == EPickupActor::PlantActor)
		{
			_InvokingWidget->SetHUDInteractText("");

			ItemComponent->Mesh->SetRenderCustomDepth(true);
		}
	}
}

