// Fill out your copyright notice in the Description page of Project Settings.


#include "SellBin.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Plant.h"
#include "Prototype2Character.h"
#include "Prototype2PlayerState.h"
#include "Components/TextBlock.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/PlayerState.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Widget_SellCropUI.h"

// Sets default values
ASellBin::ASellBin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
}

// Called when the game starts or when spawned
void ASellBin::BeginPlay()
{
	Super::BeginPlay();
	
	InterfaceType = EInterfaceType::SellBin;

	// Sell UI related
	startPosition = FVector(0, 0, 0);// SellAmountWidgetComponent->GetComponentLocation(); // Set UI start location variable
	movingTimer = movingTime; // Set starting timer to equal max time

	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	
}

// Called every frame
void ASellBin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	
	MoveUIComponent(DeltaTime);
}

bool ASellBin::IsInteractable(APrototype2PlayerState* player)
{
	if (!player)
		return false;

	if (auto controller = player->GetPlayerController())
	{
		if (auto character = controller->GetCharacter())
		{
			if (auto casted = Cast<APrototype2Character>(character))
			{
				if (casted->HeldItem)
				{
					if (Cast<APlant>(casted->HeldItem))
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

void ASellBin::FireSellFX(APlant* _Plant, APrototype2Character* player)
{
	if (SellAmountWidgetComponent)
	{
		if (player->IsLocallyControlled() && _Plant)
		{
			// Selling UI - Show in-game UI when selling
			if (SellAmountWidgetComponent->GetWidget())
			{
				SellAmountWidgetComponent->SetRelativeLocation(FVector(startPosition)); // Reset text to start position
				
				if (auto sellCropUI = Cast<UWidget_SellCropUI>(SellAmountWidgetComponent->GetWidget()))
				{
					sellCropUI->SetCropValue(_Plant->ItemComponent->CropValue);
					if (sellCropUI->SellText)
					{
						sellCropUI->SellText->SetVisibility(ESlateVisibility::Visible);
					}
					isMoving = true;
				}
			}
		}
	}
}

void ASellBin::Client_MoveUI_Implementation(float DeltaTime)
{
	if (isMoving == true)
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
			if (auto sellCropUI = Cast<UWidget_SellCropUI>(SellAmountWidgetComponent->GetWidget()))
			{
				if (sellCropUI->SellText)
				{
					sellCropUI->SellText->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}
}

void ASellBin::ClientInteract(APrototype2Character* player)
{
	if (player->HeldItem)
	{
		if (auto Plant = Cast<APlant>(player->HeldItem))
		{
			bWidgetVisible = true;
			FireSellFX(Plant, player);
			player->UpdateDecalDirection(false);
			if (player->PlayerHUDRef)
				player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
		}
	}
}

void ASellBin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASellBin::MoveUIComponent(float _dt)
{
	if (SellAmountWidgetComponent)
	{
		if (movingTimer > 0)
		{
			movingTimer -= _dt; // Decrease timer
			SellAmountWidgetComponent->AddLocalOffset(FVector(0, 0, moveSpeed * _dt)); // Move component
		}
		else
		{
			movingTimer = movingTime;
			isMoving = false;
			HideParticleSystem();
		}
	}
}

void ASellBin::Interact(APrototype2Character* player)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempted to sell something!"));
	if (player->HeldItem)
	{
		if (auto Plant = Cast<APlant>(player->HeldItem))
		{
			UE_LOG(LogTemp, Warning, TEXT("Tis A Plant!"));
			
			// Audio
			if (player->SellCue)
			{
				player->PlaySoundAtLocation(GetActorLocation(), player->SellCue);
			}

			UE_LOG(LogTemp, Warning, TEXT("Gimmi %s gold"), *FString::FromInt(Plant->ItemComponent->CropValue));
			//Cast<APrototype2PlayerState>(player->GetPlayerState())->Coins += plant->ItemComponent->CropValue; // Previous way - increased crop value directly
			Cast<APrototype2PlayerState>(player->GetPlayerState())->ExtraCoins = Plant->ItemComponent->CropValue;
			Cast<APrototype2PlayerState>(player->GetPlayerState())->IsShowingExtraCoins = true; 

			// Reset player speed incase of gold plant
			player->bIsHoldingGold = false;
			
			// Destroy the crop the player is holding
			player->HeldItem->Destroy();
			player->HeldItem = nullptr;

			if (player->PlayerHUDRef)
			{
				player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
				player->PlayerHUDRef->SetHUDInteractText("");
			}
			player->EnableStencil(false);
			ItemComponent->Mesh->SetRenderCustomDepth(false);
			
			//Server_FireParticleSystem();
		}
	}
}

void ASellBin::OnDisplayInteractText(UWidget_PlayerHUD* InvokingWidget,APrototype2Character* _Owner, int _PlayerID)
{
	if(auto heldItem = _Owner->HeldItem)
	{
		if (heldItem->ItemComponent->PickupType == EPickup::Cabbage ||
			heldItem->ItemComponent->PickupType == EPickup::Carrot ||
			heldItem->ItemComponent->PickupType == EPickup::Mandrake ||
			heldItem->ItemComponent->PickupType == EPickup::Broccoli ||
			heldItem->ItemComponent->PickupType == EPickup::Daikon ||
			heldItem->ItemComponent->PickupType == EPickup::Radish)
		{
			
			InvokingWidget->SetHUDInteractText("Sell");

			_Owner->EnableStencil(true);
		}
	}
}

