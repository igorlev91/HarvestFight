// Fill out your copyright notice in the Description page of Project Settings.


#include "GrowSpot.h"
#include "GrowSpot.h"
#include "Plant.h"
#include "Prototype2Character.h"
#include "Prototype2PlayerState.h"
#include "Seed.h"
#include "GrowableWeapon.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapon.h"

// Sets default values
AGrowSpot::AGrowSpot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	bReplicates = true;

	InteractSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	InteractSystem->SetupAttachment(RootComponent);

	InterfaceType = EInterfaceType::GrowSpot;
}

void AGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGrowSpot, growTimer);
	DOREPLIFETIME(AGrowSpot, growTime);
	DOREPLIFETIME(AGrowSpot, plant);
	DOREPLIFETIME(AGrowSpot, weapon);
	DOREPLIFETIME(AGrowSpot, GrowSpotState);
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();

	//InterfaceType = EInterfaceType::GrowSpot;
	//GrowSpotState = EGrowSpotState::Empty;

	//ItemComponent->Mesh->SetCollisionProfileName("OverlapAll");

	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	if (ParticleSystem)
	{
		InteractSystem->SetupAttachment(RootComponent);
		InteractSystem->SetAsset(ParticleSystem);
	}
}

void AGrowSpot::Multi_Plant_Implementation()
{
	if (plant)
	{
		if (plant->ItemComponent)
		{
			if (plant->ItemComponent->Mesh)
			{
				plant->ItemComponent->Mesh->SetSimulatePhysics(false);
				plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	if (weapon)
	{
		if (weapon->ItemComponent)
		{
			if (weapon->ItemComponent->Mesh)
			{
				weapon->ItemComponent->Mesh->SetSimulatePhysics(false);
				weapon->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

void AGrowSpot::Multi_FireParticleSystem_Implementation()
{
	// Spawn a one-shot emitter at the InteractSystem's location
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleSystem, InteractSystem->GetComponentLocation());
	NiagaraComponent->SetIsReplicated(true);
	// Set the NiagaraComponent to auto-destroy itself after it finishes playing
	NiagaraComponent->SetAutoDestroy(true);
}

void AGrowSpot::GrowPlantOnTick(float _deltaTime)
{
	if (growTimer > 0)
	{
		GrowSpotState = EGrowSpotState::Growing;
		growTimer -= _deltaTime;

		if (growTimer <= 0)
		{
			GrowSpotState = EGrowSpotState::Grown;
		}
	}
}

// Called every frame
void AGrowSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	if (plant)
	{
		FVector scale = FMath::Lerp<FVector>({2.0f, 2.0f, 2.0f}, {0.1f, 0.1f, 0.1f}, growTimer / growTime);
		FVector pos = FMath::Lerp<FVector>({GetActorLocation()}, GetActorLocation() + FVector::UpVector * 20.0f, growTimer / growTime);
		plant->ItemComponent->Mesh->SetSimulatePhysics(false);
		plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		plant->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		plant->ItemComponent->Mesh->SetWorldScale3D(scale);
		plant->SetActorLocation(pos);
		plant->SetActorRotation(FRotator(0,0,0));
	}
	
	if (weapon)
	{
		FVector scale = FMath::Lerp<FVector>({2.0f, 2.0f, 2.0f}, {0.1f, 0.1f, 0.1f}, growTimer / growTime);
		FVector pos = FMath::Lerp<FVector>({GetActorLocation()}, GetActorLocation() + FVector::UpVector * 20.0f, growTimer / growTime);
		weapon->ItemComponent->Mesh->SetSimulatePhysics(false);
		weapon->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		weapon->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		weapon->ItemComponent->Mesh->SetWorldScale3D(scale);
		weapon->SetActorLocation(pos);
		weapon->SetActorRotation(FRotator(0,0,0));
	}

	if (HasAuthority())
		GrowPlantOnTick(DeltaTime);
}

void AGrowSpot::Multi_GrowOnTick_Implementation(float _deltaTime)
{
	
}

void AGrowSpot::Multi_UpdateState_Implementation(EGrowSpotState _newState)
{
	GrowSpotState = _newState;
}

void AGrowSpot::Interact(APrototype2Character* player)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempted to interact with the grow spot"));
	if (player->PlayerID == Player_ID)
	{
		switch(GrowSpotState)
		{
		case EGrowSpotState::Empty:
			{
				if (auto seed = Cast<ASeed>(player->HeldItem))
				{
					UE_LOG(LogTemp, Warning, TEXT("Attempted to plant something!"));
						
					// seed has an assigned plant
					if (seed->plantToGrow)
					{
						// Audio
						if (player->PlantCue)
						{
							player->PlaySoundAtLocation(GetActorLocation(), player->PlantCue);
						}
						// Set grow time
							
						if (seed->growtime > 0)
						{
							growTimer = seed->growtime;
						}
						else if (seed->ItemComponent->GrowTime > 0)
						{
							// deprecated soon
							growTimer = seed->ItemComponent->GrowTime;
						}
						else
						{
							growTimer = 1.0f;
						}
							
						if (seed->isWeapon)
						{
							weapon = GetWorld()->SpawnActor<AGrowableWeapon>(seed->plantToGrow);
							SetWeapon(weapon, growTime);
						}
						else
						{
							plant = GetWorld()->SpawnActor<APlant>(seed->plantToGrow);
							SetPlant(plant, growTime);
						}
							
						Multi_Plant();
						
						if (seed)
							seed->Destroy();

						player->HeldItem = nullptr;

						if (player->PlayerHUDRef)
						{
							player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
							player->PlayerHUDRef->SetHUDInteractText("");
						}
						player->EnableStencil(false);

						Multi_UpdateState(EGrowSpotState::Growing);
						GrowSpotState = EGrowSpotState::Growing;
					}
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				break;
			}
		case EGrowSpotState::Grown:
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempted to Harvest something!"));
						
				if (player->HeldItem)
				{
					UE_LOG(LogTemp, Warning, TEXT("But dropped it..."));
					player->Server_DropItem();
					player->UpdateDecalDirection(true, true);
					player->HeldItem = nullptr;
					if (player->PlayerHUDRef)
					{
						player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
						player->PlayerHUDRef->SetHUDInteractText("");
					}
					player->EnableStencil(false);
					break;
				}

				if (weapon)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tis a weapon!"));
					weapon->isGrown = true;
							
					player->Server_PickupItem(weapon->ItemComponent, weapon);
					player->WeaponCurrentDurability = player->WeaponMaxDurability;
					
					//// Change the weapon UI for this player
					player->GetPlayerHUD()->UpdateWeaponUI(EPickup::Weapon);
					
					weapon->Destroy();
				}
				weapon = nullptr;
				
				if (plant)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tis a Plant!"));
					plant->isGrown = true;

					plant->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
							
					// Put players weapon on back
					player->Multi_SocketItem_Implementation(player->Weapon->Mesh, FName("WeaponHolsterSocket"));
					player->Server_PickupItem(plant->ItemComponent, plant);
					player->UpdateDecalDirection(true, true);

					player->HeldItem = plant;
					
					// Special sound for mandrake when picked up
					if (player->HeldItem)
					{
						if (player->HeldItem->ItemComponent->PickupType == EPickup::Mandrake)
						{
							if (player->MandrakeScreamCue)
							{
								if (MandrakeAttenuationSettings)
								{
									player->PlaySoundAtLocation(GetActorLocation(), player->MandrakeScreamCue, MandrakeAttenuationSettings);
								}
								else
								{
									player->PlaySoundAtLocation(GetActorLocation(), player->MandrakeScreamCue); 
								}
							}
						}
					}
				}
				plant = nullptr;
				
				if (player->PlayerHUDRef)
				{
					player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
					player->PlayerHUDRef->SetHUDInteractText("");
				}
				player->EnableStencil(false);
				ItemComponent->Mesh->SetRenderCustomDepth(false);

				UE_LOG(LogTemp, Warning, TEXT("Empty the plot."));
				Multi_UpdateState(EGrowSpotState::Empty);
				GrowSpotState = EGrowSpotState::Empty;
			}
		default:
			break;
		}
	}
}

void AGrowSpot::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerID: %s"), *FString::FromInt(_playerID))
	//UE_LOG(LogTemp, Warning, TEXT("GrowSpotID: %s"), *FString::FromInt(Player_ID))

	if (auto playerState = owner->GetPlayerState<APrototype2PlayerState>())
	{
		if (playerState->Player_ID == _playerID)
		{
			switch (GrowSpotState)
			{
			case EGrowSpotState::Empty:
				{
					// Set to "Grow"
					if(owner->HeldItem && Cast<ASeed>(owner->HeldItem))
					{
						
						_invokingWiget->SetHUDInteractText("Grow");

						owner->EnableStencil(true);
					}
					break;
				}
			case EGrowSpotState::Growing:
				{
					_invokingWiget->SetHUDInteractText("");
					owner->EnableStencil(false);
					break;
				}
			case EGrowSpotState::Grown:
				{
					if (!owner->HeldItem && (plant || weapon))
					{
						// Set to "Grow"
						_invokingWiget->SetHUDInteractText("Pick Up");
						owner->EnableStencil(true);
					}
					
					break;
				}
			default:
				{
					break;
				}
			}						
		}
	}
}

void AGrowSpot::SetPlant(APlant* _plant, float _growTime)
{
	plant = _plant;
	growTimer = _growTime;
	GrowSpotState = EGrowSpotState::Growing;
		
	if (growTimer <= 0)
	{
		growTimer = 1.0f;
	}
}

void AGrowSpot::SetWeapon(AGrowableWeapon* _weapon, float _growTime)
{
	weapon = _weapon;
	growTimer = _growTime;
	GrowSpotState = EGrowSpotState::Growing;
		
	if (growTimer <= 0)
	{
		growTimer = 1.0f;
	}
}

