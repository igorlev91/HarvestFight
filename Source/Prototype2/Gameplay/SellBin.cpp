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

	ThrowToSellCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ThrowToSellCollider"));
	ThrowToSellCollider->SetupAttachment(RootComponent);
	
	//SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void ASellBin::BeginPlay()
{
	Super::BeginPlay();

	InterfaceType = EInterfaceType::SellBin;

	// Sell UI related
	StartPosition = FVector(0, 0, 0);// SellAmountWidgetComponent->GetComponentLocation(); // Set UI start location variable
	MovingTimer = MovingTime; // Set starting timer to equal max time

	ItemComponent->Mesh->SetMobility(EComponentMobility::Movable);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	
	ItemComponent->Mesh->SetNotifyRigidBodyCollision(true);
	ItemComponent->Mesh->OnComponentHit.AddDynamic(this, &ASellBin::OnPlayerTouchSellBin);
	ItemComponent->Mesh->SetIsReplicated(true);
	
	if (SellBinData)
	{
		ItemComponent->Mesh->SetWorldScale3D(SellBinData->DesiredScale);
	}

	//SSComponent->SetMeshToStretch(ItemComponent->Mesh);
	
	ThrowToSellCollider->OnComponentBeginOverlap.AddDynamic(this, &ASellBin::SellOnThrown);
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

void ASellBin::Server_FireParticleSystem_Implementation()
{
	//Multi_FireParticleSystem();
}

void ASellBin::Multi_FireSellVFXCrop_Implementation(APrototype2Character* _Player, APlant* _Plant)
{
	APrototype2PlayerState* SomePlayerState = _Player->GetPlayerState<APrototype2PlayerState>();
	if (!SomePlayerState)
		return;

	int32 PlantSellValue = (_Plant->ServerData.SeedData->BabyStarValue * _Plant->ServerData.SeedData->PlantData->Multiplier * (_Plant->NumberOfNearbyFlowers + 1));
	PlantSellValue *= 10;
	int32 IncreaseAmount = _Plant->ItemComponent->bGold ? PlantSellValue * _Plant->ServerData.SeedData->GoldMultiplier : PlantSellValue;

	InteractSystem->SetVectorParameter(FName("Coin Colour"), SomePlayerState->Details.PureToneColour);
	InteractSystem->SetIntParameter(FName("CoinCount"), IncreaseAmount);
	InteractSystem->Activate(true);

	if (PoofSystem)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, InteractSystem->GetComponentLocation(), FRotator{});
		SpawnedVFX->SetActorScale3D(FVector::One() * 1.5f);
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}

void ASellBin::Multi_FireSellVFX_Implementation(APrototype2Character* _Player, int32 _SellAmount)
{
	APrototype2PlayerState* SomePlayerState = _Player->GetPlayerState<APrototype2PlayerState>();
	if (!SomePlayerState)
		return;

	InteractSystem->SetVectorParameter(FName("Coin Colour"), SomePlayerState->Details.PureToneColour);
	InteractSystem->SetIntParameter(FName("CoinCount"), _SellAmount);
	InteractSystem->Activate(true);

	if (PoofSystem)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, InteractSystem->GetComponentLocation(), FRotator{});
		SpawnedVFX->SetActorScale3D(FVector::One() * 1.5f);
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}

void ASellBin::Client_MoveUI_Implementation(float _DeltaTime)
{
	if (bIsMoving == true)
	{
		
	}
}

void ASellBin::Multi_FireParticleSystem_Implementation()
{
	// Spawn a one-shot emitter at the InteractSystem's location
	//UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleSystem, InteractSystem->GetComponentLocation());
	//NiagaraComponent->SetIsReplicated(true);
	//// Set the NiagaraComponent to auto-destroy itself after it finishes playing
	//NiagaraComponent->SetAutoDestroy(true);
	//NiagaraComponent->Activate();
}

void ASellBin::HideParticleSystem()
{
	if (SellAmountWidgetComponent)
	{
		bWidgetVisible = false;
		if (SellAmountWidgetComponent->GetWidget())
		{
			if (auto SellCropUI = Cast<UWidget_SellCropUI>(SellAmountWidgetComponent->GetWidget()))
			{
				if (SellCropUI->SellText)
				{
					SellCropUI->SellText->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}
}

void ASellBin::ClientInteract(APrototype2Character* _Player)
{
	// Force Bump Into SellBin
	////////
	return;
	////////
	
	if (_Player->HeldItem)
	{
		if (auto Plant = Cast<APlant>(_Player->HeldItem))
		{
			bWidgetVisible = true;
			//_Player->UpdateDecalDirection(false);
			if (_Player->PlayerHUDRef)
				_Player->PlayerHUDRef->ClearPickupUI();
		}
	}
}

void ASellBin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASellBin::OnPlayerTouchSellBin(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor->HasNetOwner())
		return;
	
	if (!OtherActor)
		return;
	
	APrototype2Character* SomePlayer = Cast<APrototype2Character>(OtherActor);
	
	if (!SomePlayer)
		return;
	
	//UE_LOG(LogTemp, Warning, TEXT("Attempted to sell something!"));
	if (SomePlayer->HeldItem)
	{
		if (auto Plant = Cast<APlant>(SomePlayer->HeldItem))
		{
			//Client_OnPlayerSell(SomePlayer);
			//SomePlayer->PlayerStateRef->Client_OnAddCoins();
			
			// Audio
			if (HasAuthority())
			{
				//SSComponent->Boing();
				Multi_FireSellVFXCrop(SomePlayer, Plant);
				
				if (SomePlayer->SellCue)
				{
					SomePlayer->Client_PlaySoundAtLocation(GetActorLocation(), SomePlayer->SellCue, nullptr);
				}

				SomePlayer->PlayerStateRef->AddCoins(Plant);
				
				SomePlayer->Multi_SocketItem(SomePlayer->WeaponMesh, FName("Base-HumanWeapon"));
				
				// Destroy the crop the player is holding
				SomePlayer->HeldItem->Destroy();
				SomePlayer->HeldItem = nullptr;
				SomePlayer->OnRep_HeldItem();
				SomePlayer->Client_RefreshCurrentMaxSpeed();

				if (auto* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
				{
					int32 PlantSellValue = (Plant->ServerData.SeedData->BabyStarValue * Plant->ServerData.SeedData->PlantData->Multiplier * (Plant->NumberOfNearbyFlowers + 1));
					PlantSellValue *= GameStateCast->SellMultiplier;
					int32 IncreaseAmount = Plant->ItemComponent->bGold ? PlantSellValue * Plant->ServerData.SeedData->GoldMultiplier : PlantSellValue;
					Multi_OnPlayerSell(SomePlayer, IncreaseAmount);
				}
			}
		}
	}
}

void ASellBin::MoveUIComponent(float _Dt)
{
	if (SellAmountWidgetComponent)
	{
		if (MovingTimer > 0)
		{
			MovingTimer -= _Dt; // Decrease timer
			SellAmountWidgetComponent->AddLocalOffset(FVector(0, 0, MoveSpeed * _Dt)); // Move component
		}
		else
		{
			MovingTimer = MovingTime;
			bIsMoving = false;
			HideParticleSystem();
		}
	}
}

void ASellBin::SellOnThrown(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor)
		return;

	UKismetSystemLibrary::PrintString(GetWorld(), "SellOnThrow()");
	if (APlant* ThrownPlant = Cast<APlant>(OtherActor))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Cast Successful");
		if (ThrownPlant->ItemComponent->PlayerWhoThrewItem)
		{
			UKismetSystemLibrary::PrintString(GetWorld(), "Correct PlayerWhoeThrew");
			if (auto* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
			{
				int32 PlantSellValue = (ThrownPlant->ServerData.SeedData->BabyStarValue * ThrownPlant->ServerData.SeedData->PlantData->Multiplier * (ThrownPlant->NumberOfNearbyFlowers + 1));
				PlantSellValue *= GameStateCast->SellMultiplier;
				int32 IncreaseAmount = ThrownPlant->ItemComponent->bGold ? PlantSellValue * ThrownPlant->ServerData.SeedData->GoldMultiplier : PlantSellValue;
				Client_OnPlayerSell(ThrownPlant->ItemComponent->PlayerWhoThrewItem, IncreaseAmount);
				ThrownPlant->ItemComponent->PlayerWhoThrewItem->PlayerStateRef->Client_OnAddCoins(IncreaseAmount);
			}
			
		
			// Audio
			if (HasAuthority())
			{
				//SSComponent->Boing();
				Multi_FireSellVFXCrop(ThrownPlant->ItemComponent->PlayerWhoThrewItem, ThrownPlant);
			
				if (ThrownPlant->ItemComponent->PlayerWhoThrewItem->SellCue)
				{
					ThrownPlant->ItemComponent->PlayerWhoThrewItem->Client_PlaySoundAtLocation(GetActorLocation(), ThrownPlant->ItemComponent->PlayerWhoThrewItem->SellCue, nullptr);
				}
				ThrownPlant->ItemComponent->PlayerWhoThrewItem->PlayerStateRef->AddCoins(ThrownPlant);
				
				// Destroy the crop the player is holding
				ThrownPlant->Destroy();
			}
		}
	}
}

void ASellBin::Client_Boing_Implementation()
{
	//SSComponent->Boing();
}

void ASellBin::Server_OnPlayerSell_Implementation(APrototype2Character* _Player, APlant* _Plant)
{
	if (_Player->SellCue)
	{
		_Player->Client_PlaySoundAtLocation(GetActorLocation(), _Player->SellCue, nullptr);
	}
				
	_Player->PlayerStateRef->AddCoins(_Plant);
			
	_Player->Multi_SocketItem(_Player->WeaponMesh, FName("Base-HumanWeapon"));
				
	// Destroy the crop the player is holding
	_Player->HeldItem->Destroy();
	_Player->HeldItem = nullptr;
	_Player->OnRep_HeldItem();
	_Player->Client_RefreshCurrentMaxSpeed();
}

void ASellBin::Client_OnPlayerSell_Implementation(APrototype2Character* _Player, int32 _Points)
{
	if (_Player->PlayerHUDRef)
		_Player->PlayerHUDRef->ClearPickupUI();
	
	ItemComponent->SetStencilEnabled(false);

	APrototype2Gamestate* GamestateCast = Cast<APrototype2Gamestate>(GetWorld()->GetGameState());
	if (!GamestateCast)
		return;

	for (int i = 0; i < GamestateCast->Server_Players.Num(); i++)
	{
		if (_Player->PlayerStateRef->PlayerName == GamestateCast->Server_Players[i]->PlayerName)
		{
			OnItemSoldDelegate.Broadcast(i, _Points);
			return;
		}
	}
	
	//OnItemSoldDelegate.Broadcast(_Player->PlayerStateRef->Player_ID);
}

void ASellBin::Multi_OnPlayerSell_Implementation(APrototype2Character* _Player, int32 _Points)
{
	APrototype2Gamestate* GamestateCast = Cast<APrototype2Gamestate>(GetWorld()->GetGameState());
	if (!GamestateCast)
		return;

	for (int i = 0; i < GamestateCast->Server_Players.Num(); i++)
	{
		if (_Player->PlayerStateRef->PlayerName == GamestateCast->Server_Players[i]->PlayerName)
		{
			OnItemSoldDelegate.Broadcast(i, _Points);
			return;
		}
	}
}

void ASellBin::Interact(APrototype2Character* _Player)
{
	// Force Bump Into SellBin
	////////
	return;
	////////
	
	APrototype2Gamestate* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameStateCast)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to sell something!"));
	if (_Player->HeldItem)
	{
		if (auto Plant = Cast<APlant>(_Player->HeldItem))
		{
			UE_LOG(LogTemp, Warning, TEXT("Tis A Plant!"));
			
			// Audio
			if (_Player->SellCue)
			{
				_Player->PlaySoundAtLocation(GetActorLocation(), _Player->SellCue);
			}

			_Player->PlayerStateRef->AddCoins(Plant);
			
			// Destroy the crop the player is holding
			_Player->HeldItem->Destroy();
			_Player->HeldItem = nullptr;

			if (_Player->PlayerHUDRef)
			{
				_Player->PlayerHUDRef->ClearPickupUI();
				_Player->PlayerHUDRef->SetHUDInteractText("");
			}
			_Player->EnableStencil(false);
			ItemComponent->Mesh->SetRenderCustomDepth(false);

			_Player->RefreshCurrentMaxSpeed();
		}
	}
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

