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

	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
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

	if (HasAuthority())
		SSComponent->SetMeshToStretch(ItemComponent->Mesh);
}

void ASellBin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	
	//MoveUIComponent(DeltaTime);
}

bool ASellBin::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;

	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Casted = Cast<APrototype2Character>(Character))
			{
				if (Casted->HeldItem)
				{
					if (Cast<APlant>(Casted->HeldItem))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

void ASellBin::Server_FireParticleSystem_Implementation()
{
	//Multi_FireParticleSystem();
}

void ASellBin::FireSellFX(APlant* _Plant, APrototype2Character* _Player)
{
	if (SellAmountWidgetComponent)
	{
		if (_Player->IsLocallyControlled() && _Plant)
		{
			// Selling UI - Show in-game UI when selling
			if (SellAmountWidgetComponent->GetWidget())
			{
				SellAmountWidgetComponent->SetRelativeLocation(FVector(StartPosition)); // Reset text to start position
				
				if (auto sellCropUI = Cast<UWidget_SellCropUI>(SellAmountWidgetComponent->GetWidget()))
				{
					if (_Plant->SeedData->PlantData)
					{
						sellCropUI->SetCropValue(_Plant->SeedData->BabyStarValue * _Plant->SeedData->PlantData->Multiplier);
					}
					if (sellCropUI->SellText)
					{
						sellCropUI->SellText->SetVisibility(ESlateVisibility::Visible);
					}
					bIsMoving = true;
				}
			}
		}
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
			FireSellFX(Plant, _Player);
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
	APrototype2Gamestate* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameStateCast)
		return;
	
	//UE_LOG(LogTemp, Warning, TEXT("Attempted to sell something!"));
	if (SomePlayer->HeldItem)
	{
		if (auto Plant = Cast<APlant>(SomePlayer->HeldItem))
		{
			Client_OnPlayerSell(SomePlayer);
			
			// Audio
			if (HasAuthority())
			{
				SSComponent->Boing();
				
				if (SomePlayer->SellCue)
				{
					SomePlayer->PlaySoundAtLocation(GetActorLocation(), SomePlayer->SellCue, nullptr);
				}

				if (!SomePlayer->PlayerStateRef->IsWinning() && GameStateCast->MatchLengthSeconds < LoosingPlayerCanAddMatchTimeTime)
				{
					GameStateCast->MatchLengthSeconds += 15;
					if (GameStateCast->MatchLengthSeconds >= 60)
					{
						GameStateCast->MatchLengthSeconds = 0;
						GameStateCast->MatchLengthMinutes++;
					}
				}


				SomePlayer->PlayerStateRef->AddCoins(Plant);

				// Reset player speed incase of gold plant
				SomePlayer->bIsHoldingGold = false;
			
				SomePlayer->Multi_SocketItem(SomePlayer->WeaponMesh, FName("Base-HumanWeapon"));
				
				// Destroy the crop the player is holding
				SomePlayer->HeldItem->Destroy();
				SomePlayer->HeldItem = nullptr;
				SomePlayer->Client_RefreshCurrentMaxSpeed();
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

void ASellBin::Client_Boing_Implementation()
{
	SSComponent->Boing();
}

void ASellBin::Server_OnPlayerSell_Implementation(APrototype2Character* _Player, APlant* _Plant)
{
	if (_Player->SellCue)
	{
		_Player->PlaySoundAtLocation(GetActorLocation(), _Player->SellCue, nullptr);
	}
				
	_Player->PlayerStateRef->AddCoins(_Plant);

	// Reset player speed incase of gold plant
	_Player->bIsHoldingGold = false;
			
	_Player->Multi_SocketItem(_Player->WeaponMesh, FName("Base-HumanWeapon"));
				
	// Destroy the crop the player is holding
	_Player->HeldItem->Destroy();
	_Player->HeldItem = nullptr;
	_Player->Client_RefreshCurrentMaxSpeed();
}

void ASellBin::Client_OnPlayerSell_Implementation(APrototype2Character* _Player)
{
	if (_Player->PlayerHUDRef)
		_Player->PlayerHUDRef->ClearPickupUI();

	_Player->bIsHoldingGold = false;
	ItemComponent->SetStencilEnabled(false);
	OnItemSoldDelegate.Broadcast(_Player->PlayerStateRef->Player_ID);
}

void ASellBin::Multi_OnPlayerSell_Implementation(APrototype2Character* _Player)
{
	
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

			// Reset player speed incase of gold plant
			_Player->bIsHoldingGold = false;
			
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

void ASellBin::HoldInteract(APrototype2Character* _Player)
{
}

void ASellBin::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int _PlayerID)
{
	if(auto HeldItem = _Owner->HeldItem)
	{
		if (HeldItem->PickupActor == EPickupActor::PlantActor)
		{
			_InvokingWidget->SetHUDInteractText("");

			ItemComponent->Mesh->SetRenderCustomDepth(true);
		}
	}
}

