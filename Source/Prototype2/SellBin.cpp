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
	SellAmountWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SellAmountWidgetComponent"));
	SellAmountWidgetComponent->SetupAttachment(RootComponent);
	SellAmountWidgetComponent->SetIsReplicated(false);
	
	InterfaceType = EInterfaceType::SellBin;

	InteractSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	InteractSystem->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASellBin::BeginPlay()
{
	Super::BeginPlay();

	// Sell UI related
	startPosition = FVector(0, 0, 0);// SellAmountWidgetComponent->GetComponentLocation(); // Set UI start location variable
	movingTimer = movingTime; // Set starting timer to equal max time

	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

// Called every frame
void ASellBin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveUIComponent(DeltaTime);
}

void ASellBin::Server_FireParticleSystem_Implementation()
{
	//Multi_FireParticleSystem();
}

void ASellBin::FireSellFX(APlant* _plant, APrototype2Character* player)
{
	if (player->IsLocallyControlled())
	{
		// Selling UI - Show in-game UI when selling
		if (SellAmountWidgetComponent->GetWidget())
		{
			SellAmountWidgetComponent->SetRelativeLocation(FVector(startPosition)); // Reset text to start position
				
			if (auto* SellCropUI = Cast<UWidget_SellCropUI>(SellAmountWidgetComponent->GetWidget()))
			{
				SellCropUI->SetCropValue(_plant->ItemComponent->CropValue);
				SellCropUI->SellText->SetVisibility(ESlateVisibility::Visible);
				isMoving = true;
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
	bWidgetVisible = false;
	if (SellAmountWidgetComponent->GetWidget())
	{
		if (auto* SellCropUI = Cast<UWidget_SellCropUI>(SellAmountWidgetComponent->GetWidget()))
		{
			SellCropUI->SellText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ASellBin::ClientInteract(APrototype2Character* player)
{
	if (player->HeldItem)
	{
		if (auto* plant = Cast<APlant>(player->HeldItem))
		{
			bWidgetVisible = true;
			FireSellFX(plant, player);
		}
	}
}

void ASellBin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASellBin, bWidgetVisible);
}

void ASellBin::MoveUIComponent(float _dt)
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

void ASellBin::Interact(APrototype2Character* player)
{
	if (player->HeldItem)
	{
		if (auto* plant = Cast<APlant>(player->HeldItem))
		{
			// Audio
			if (player->SellCue)
			{
				player->PlaySoundAtLocation(GetActorLocation(), player->SellCue);
			}
			
			//Cast<APrototype2PlayerState>(player->GetPlayerState())->Coins += plant->ItemComponent->CropValue; // Previous way - increased crop value directly
			Cast<APrototype2PlayerState>(player->GetPlayerState())->ExtraCoins = plant->ItemComponent->CropValue;
			Cast<APrototype2PlayerState>(player->GetPlayerState())->IsShowingExtraCoins = true; 

			// Destroy the crop the player is holding
			player->HeldItem->Destroy();
			player->HeldItem = nullptr;
			
			//Server_FireParticleSystem();


		}
	}
}

void ASellBin::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	if(auto heldItem = owner->HeldItem)
	{
		if (heldItem->ItemComponent->PickupType == EPickup::Cabbage ||
			heldItem->ItemComponent->PickupType == EPickup::Carrot ||
			heldItem->ItemComponent->PickupType == EPickup::Mandrake)
		{
			_invokingWiget->SetHUDInteractText("Sell");
		}
	}
}

