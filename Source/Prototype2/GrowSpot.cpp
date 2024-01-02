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
}

void AGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGrowSpot, growTimer);
	DOREPLIFETIME(AGrowSpot, growTime);
	DOREPLIFETIME(AGrowSpot, growingPlant);
	DOREPLIFETIME(AGrowSpot, plantGrown);
	DOREPLIFETIME(AGrowSpot, plant);
	DOREPLIFETIME(AGrowSpot, weapon);
	DOREPLIFETIME(AGrowSpot, GrowSpotState);
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();

	InterfaceType = EInterfaceType::GrowSpot;
	GrowSpotState = EGrowSpotState::Empty;

	ItemComponent->Mesh->SetCollisionProfileName("OverlapAll");

	if (ParticleSystem)
	{
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
	if (growingPlant && HasAuthority())
	{
		if (growTimer > 0)
		{
			GrowSpotState = EGrowSpotState::Growing;
			growTimer -= _deltaTime;

			if (growTimer <= 0)
			{
				Multi_FireParticleSystem();
				//InteractSystem->Activate();
				//InteractSystem->ResetSystem();
				
				plantGrown = true;
				growingPlant = false;
				GrowSpotState = EGrowSpotState::Grown;
			}
		}
			
		
	}
	
	if (plant)
	{
		FVector scale = FMath::Lerp<FVector>({2.0f, 2.0f, 2.0f}, {0.1f, 0.1f, 0.1f}, growTimer / growTime);
		FVector pos = FMath::Lerp<FVector>({GetActorLocation()}, GetActorLocation() + FVector::UpVector * 10.0f, growTimer / growTime);
		ItemComponent->Mesh->SetCollisionProfileName("OverlapAll");
		plant->ItemComponent->Mesh->SetSimulatePhysics(false);
		plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		plant->ItemComponent->Mesh->SetWorldScale3D(scale);
		plant->SetActorLocation(pos);
		plant->SetActorRotation(FRotator(0,0,0));
	}

	if (weapon)
	{
		FVector scale = FMath::Lerp<FVector>({2.0f, 2.0f, 2.0f}, {0.1f, 0.1f, 0.1f}, growTimer / growTime);
		FVector pos = FMath::Lerp<FVector>({GetActorLocation()}, GetActorLocation() + FVector::UpVector * 10.0f, growTimer / growTime);
		ItemComponent->Mesh->SetCollisionProfileName("OverlapAll");
		weapon->ItemComponent->Mesh->SetSimulatePhysics(false);
		weapon->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		weapon->ItemComponent->Mesh->SetWorldScale3D(scale);
		weapon->SetActorLocation(pos);
		weapon->SetActorRotation(FRotator(0,0,0));
	}
}

// Called every frame
void AGrowSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GrowPlantOnTick(DeltaTime);

	switch(GrowSpotState)
	{
	case EGrowSpotState::Growing:
		{
			InteractSystem->Deactivate();
			if (plant)
				plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (plant)
			{
				plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				plant->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			}
				
			ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			ItemComponent->Mesh->SetCollisionProfileName("OverlapAll");
			break;
		}
		default:
			break;
	}
}

void AGrowSpot::Interact(APrototype2Character* player)
{
	if (HasAuthority())
	{
		if (auto* playerState = player->GetPlayerState<APrototype2PlayerState>())
		{
			if (playerState->Player_ID == Player_ID)
			{
				if (auto* seed = Cast<ASeed>(player->HeldItem))
				{
					if (!plant && GrowSpotState == EGrowSpotState::Empty)
					{
						if (seed->plantToGrow)
						{
							// Audio
							if (player->PlantCue)
							{
								player->PlaySoundAtLocation(GetActorLocation(), player->PlantCue);
							}
							
							if (seed->isWeapon)
							{
								//Multi_FireParticleSystem();
								growTime = player->HeldItem->ItemComponent->GrowTime;
								SetWeapon(GetWorld()->SpawnActor<AGrowableWeapon>(seed->plantToGrow), growTime);
								Multi_Plant();
						
								if (seed)
									seed->Destroy();

								// Seed is now planted so remove from player
								player->HeldItem = nullptr;
							}
							else
							{
								//Multi_FireParticleSystem();
								growTime = player->HeldItem->ItemComponent->GrowTime;
								SetPlant(GetWorld()->SpawnActor<APlant>(seed->plantToGrow), growTime);
								Multi_Plant();
						
								if (seed)
									seed->Destroy();

								// Seed is now planted so remove from player
								player->HeldItem = nullptr;
							}
						}
					}
				}
				else if (weapon)
				{
					if (plantGrown && GrowSpotState == EGrowSpotState::Grown)
					{
						// replace here with weapon equip
						player->Server_PickupItem(weapon->ItemComponent, weapon);
						player->WeaponCurrentDurability = player->WeaponMaxDurability;
						
						//// Change the weapon UI for this player
						//player->GetPlayerHUD()->UpdateWeaponUI(EPickup::Weapon);
						
						Multi_FireParticleSystem();
						weapon->isGrown = true;
						
						// Destroy the growable weapon
						weapon->Destroy();
						
						weapon = nullptr;
						plantGrown = false;
						growingPlant = false;
						growTimer = 0.0f;
						GrowSpotState = EGrowSpotState::Empty;

					}
				}
				else if (plant && !player->HeldItem)
				{
					if (plantGrown && GrowSpotState == EGrowSpotState::Grown)
					{
						// Put players weapon on back
						player->Multi_SocketItem_Implementation(player->Weapon->Mesh, FName("WeaponHolsterSocket"));
						
						Multi_FireParticleSystem();
						player->HeldItem = plant;
						player->Server_PickupItem(plant->ItemComponent, plant);
						plant->isGrown = true;
						plant = nullptr;
						plantGrown = false;
						growingPlant = false;
						growTimer = 0.0f;
						GrowSpotState = EGrowSpotState::Empty;

						// Special sound for mandrake when picked up
						if (player->HeldItem)
						{
							if (player->HeldItem->ItemComponent->PickupType == EPickup::Mandrake)
							{
								if (player->MandrakeScreamCue)
								{
									player->PlaySoundAtLocation(GetActorLocation(), player->MandrakeScreamCue);
								}
							}
						}
					}
				}
				else if (player->HeldItem)
				{
					player->Server_DropItem();
				}
			}
		}
	}
}

void AGrowSpot::OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerID: %s"), *FString::FromInt(_playerID))
	//UE_LOG(LogTemp, Warning, TEXT("GrowSpotID: %s"), *FString::FromInt(Player_ID))
		
	if (Player_ID == _playerID)
	{
		switch (GrowSpotState)
		{
		case EGrowSpotState::Empty:
			{
				// Set to "Grow"
				if(owner->HeldItem && !Cast<APlant>(owner->HeldItem))
				{
					_invokingWiget->SetHUDInteractText("Grow");
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				_invokingWiget->SetHUDInteractText("");
				break;
			}
		case EGrowSpotState::Grown:
			{
				if (!owner->HeldItem)
				{
					// Set to "Grow"
					_invokingWiget->SetHUDInteractText("Pick Up");
				}
				break;
			}
		case EGrowSpotState::Default:
			{
				// Pass through
				break;
			}
		default:
			{
				// Set to none
				break;
			}
		}						
	}
}

void AGrowSpot::SetPlant(APlant* _plant, float _growTime)
{
	if (!plant)
	{
		plantGrown = false;
		plant = _plant;
		growTimer = _growTime;
		growingPlant = true;
		GrowSpotState = EGrowSpotState::Growing;
		if (growTimer == 0)
		{
			growTimer = 1;
		}
	}
}

void AGrowSpot::SetWeapon(AGrowableWeapon* _weapon, float _growTime)
{
	if (!weapon)
	{
		plantGrown = false;
		weapon = _weapon;
		growTimer = _growTime;
		growingPlant = true;
		GrowSpotState = EGrowSpotState::Growing;
		if (growTimer == 0)
		{
			growTimer = 1;
		}
	}
}

