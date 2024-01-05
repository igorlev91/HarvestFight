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

	PlantReadySparkle_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Attack Component"));
	PlantReadySparkle_NiagaraComponent->SetupAttachment(RootComponent);
}

void AGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(_OutLifetimeProps);
	TArray<FLifetimeProperty> OutLifetimeProps = _OutLifetimeProps;
	DOREPLIFETIME(AGrowSpot, GrowTimer);
	DOREPLIFETIME(AGrowSpot, GrowTime);
	DOREPLIFETIME(AGrowSpot, Plant);
	DOREPLIFETIME(AGrowSpot, Weapon);
	DOREPLIFETIME(AGrowSpot, GrowSpotState);
	DOREPLIFETIME(AGrowSpot, Player_ID);
	DOREPLIFETIME(AGrowSpot, PlantReadySparkle_NiagaraComponent);
}

bool AGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;
	
	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Castged = Cast<APrototype2Character>(Character))
			{
				if (_Player->Player_ID == Player_ID)
				{
					switch(GrowSpotState)
					{
					case EGrowSpotState::Empty:
						{
							if (Cast<ASeed>(Castged->HeldItem))
							{
								return true;
							}
							break;
						}
					case EGrowSpotState::Growing:
						{
							break;
						}
					case EGrowSpotState::Grown:
						{
							return true;
						}
					default:
						break;
					}
				}
			}
		}
	}
	

	return false;
}

void AGrowSpot::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);

	if (_Player->PlayerID == Player_ID)
	{
		switch(GrowSpotState)
		{
		case EGrowSpotState::Empty:
			{
				if (Cast<ASeed>(_Player->HeldItem))
				{
					if (_Player->PlayerHUDRef)
						_Player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
					_Player->UpdateDecalDirection(false, false);
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				break;
			}
		case EGrowSpotState::Grown:
			{
				if (Plant)
					_Player->UpdateDecalDirection(true, true);
				else
					_Player->UpdateDecalDirection(false);
			}
		default:
			break;
		}
	}
	
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
	
	if (PlantReadySparkle_NiagaraComponent)
	{
		PlantReadySparkle_NiagaraComponent->SetAsset(PlantReadySparkle_NiagaraSystem);
	}
}

void AGrowSpot::Multi_Plant_Implementation()
{
	if (Plant)
	{
		if (Plant->ItemComponent)
		{
			if (Plant->ItemComponent->Mesh)
			{
				Plant->ItemComponent->Mesh->SetSimulatePhysics(false);
				Plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	if (Weapon)
	{
		if (Weapon->ItemComponent)
		{
			if (Weapon->ItemComponent->Mesh)
			{
				Weapon->ItemComponent->Mesh->SetSimulatePhysics(false);
				Weapon->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AGrowSpot::GrowPlantOnTick(float _DeltaTime)
{
	if (GrowTimer > 0)
	{
		GrowSpotState = EGrowSpotState::Growing;
		GrowTimer -= _DeltaTime;

		if (GrowTimer <= 0)
		{
			GrowSpotState = EGrowSpotState::Grown;

			if (Plant)
			{
				Plant->bGrown = true;
			}
			if (Weapon)
			{
				Weapon->bGrown = true;
			}
		}
	}
}

void AGrowSpot::ScalePlant(APlant* _Plant, FVector _TargetScale, float _PosOffset) const
{
	const FVector Scale = FMath::Lerp<FVector>({2.0f, 2.0f, 2.0f}, _TargetScale, GrowTimer / GrowTime);
	const FVector Pos = FMath::Lerp<FVector>({GetActorLocation()}, GetActorLocation() + FVector::UpVector * _PosOffset, GrowTimer / GrowTime);
	_Plant->ItemComponent->Mesh->SetSimulatePhysics(false);
	_Plant->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_Plant->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	_Plant->ItemComponent->Mesh->SetWorldScale3D(Scale);
	_Plant->SetActorLocation(Pos);
	_Plant->SetActorRotation(FRotator(0,0,0));
}


// Called every frame
void AGrowSpot::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	if (Plant)
	{
		ScalePlant(Plant, {0.3f, 0.3f, 0.3f}, 0.0f);
	}
	
	if (Weapon)
	{
		ScalePlant(Weapon, {0.1f, 0.1f, 0.1f}, 20.0f);
	}

	if (HasAuthority())
		GrowPlantOnTick(_DeltaTime);

	// VFX
	if (GrowSpotState == EGrowSpotState::Grown)
	{
		Multi_SetPlantReadySparkle_Implementation(true);
	}
	else
	{
		Multi_SetPlantReadySparkle_Implementation(false);
	}

	
}

void AGrowSpot::Multi_GrowOnTick_Implementation(float _DeltaTime)
{
	
}

void AGrowSpot::Multi_UpdateState_Implementation(EGrowSpotState _NewState)
{
	GrowSpotState = _NewState;
}

void AGrowSpot::MandrakePickupNoise(APrototype2Character* _Player)
{
	if (!_Player->HeldItem)
	{
		return;
	}

	if (_Player->HeldItem->ItemComponent->PickupType != EPickup::Mandrake)
	{
		return;
	}

	if (_Player->MandrakeScreamCue)
	{
		if (MandrakeAttenuationSettings)
		{
			_Player->PlaySoundAtLocation(GetActorLocation(), _Player->MandrakeScreamCue, MandrakeAttenuationSettings);
		}
		else
		{
			_Player->PlaySoundAtLocation(GetActorLocation(), _Player->MandrakeScreamCue); 
		}
	}
}

void AGrowSpot::Interact(APrototype2Character* _Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempted to interact with the grow spot"));
	if (_Player->PlayerID != Player_ID)
	{
		return;
	}
	
	switch(GrowSpotState)
		{
		case EGrowSpotState::Empty:
			{
				if (auto Seed = Cast<ASeed>(_Player->HeldItem))
				{
					UE_LOG(LogTemp, Warning, TEXT("Attempted to plant something!"));
						
					// seed has an assigned plant
					if (!Seed->PlantToGrow)
					{
						return;
					}

					// Audio
					if (_Player->PlantCue)
					{
						_Player->PlaySoundAtLocation(GetActorLocation(), _Player->PlantCue);
					}
					
					// Set grow time
					if (Seed->GrowTime > 0)
					{
						GrowTimer = Seed->GrowTime;
					}
					else if (Seed->ItemComponent->GrowTime > 0)
					{
						// deprecated soon
						GrowTimer = Seed->ItemComponent->GrowTime;
					}
					else
					{
						GrowTimer = 1.0f;
					}
							
					if (Seed->bIsWeapon)
					{
						Weapon = GetWorld()->SpawnActor<AGrowableWeapon>(Seed->PlantToGrow);
						SetWeapon(Weapon, GrowTime);
					}
					else
					{
						Plant = GetWorld()->SpawnActor<APlant>(Seed->PlantToGrow);
						SetPlant(Plant, GrowTime);
					}
							
					Multi_Plant();
						
					if (Seed)
						Seed->Destroy();

					_Player->HeldItem = nullptr;

					if (_Player->PlayerHUDRef)
					{
						_Player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
						_Player->PlayerHUDRef->SetHUDInteractText("");
					}
					_Player->EnableStencil(false);

					Multi_UpdateState(EGrowSpotState::Growing);
					GrowSpotState = EGrowSpotState::Growing;
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
						
				if (_Player->HeldItem)
				{
					UE_LOG(LogTemp, Warning, TEXT("But dropped it..."));
					_Player->Server_DropItem();
					
					//Player->UpdateDecalDirection(true, true);
					_Player->HeldItem = nullptr;
					if (_Player->PlayerHUDRef)
					{
						_Player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
						_Player->PlayerHUDRef->SetHUDInteractText("");
					}
					_Player->EnableStencil(false);
					break;
				}

				if (Weapon)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tis a weapon!"));
					Weapon->bGrown = true;
							
					_Player->Server_PickupItem(Weapon->ItemComponent, Weapon);
					_Player->WeaponCurrentDurability = _Player->WeaponMaxDurability;
					
					//// Change the weapon UI for this player
					_Player->GetPlayerHUD()->UpdateWeaponUI(EPickup::Weapon);
					
					Weapon->Destroy();
				}
				Weapon = nullptr;
				
				if (Plant)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tis a Plant!"));
					Plant->bGrown = true;

					Plant->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
							
					// Put players weapon on back
					_Player->Multi_SocketItem_Implementation(_Player->Weapon->Mesh, FName("WeaponHolsterSocket"));
					_Player->Server_PickupItem(Plant->ItemComponent, Plant);
					//_Player->UpdateDecalDirection(true, true);

					_Player->HeldItem = Plant;
					
					// Special sound for mandrake when picked up
					MandrakePickupNoise(_Player);
				}
				Plant = nullptr;
				
				if (_Player->PlayerHUDRef)
				{
					_Player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
					_Player->PlayerHUDRef->SetHUDInteractText("");
				}
				_Player->EnableStencil(false);
				ItemComponent->Mesh->SetRenderCustomDepth(false);

				UE_LOG(LogTemp, Warning, TEXT("Empty the plot."));
				Multi_UpdateState(EGrowSpotState::Empty);
				GrowSpotState = EGrowSpotState::Empty;
			}
		default:
			break;
		}
}

void AGrowSpot::OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerID: %s"), *FString::FromInt(_PlayerID))
	//UE_LOG(LogTemp, Warning, TEXT("GrowSpotID: %s"), *FString::FromInt(Player_ID))

	if (auto PlayerState = _Owner->GetPlayerState<APrototype2PlayerState>())
	{
		if (PlayerState->Player_ID != _PlayerID)
		{
			return;	
		}
	}
	else
	{
		return;
	}

	switch (GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			// Set to "Grow"
			if(_Owner->HeldItem && Cast<ASeed>(_Owner->HeldItem))
			{
						
				_InvokingWidget->SetHUDInteractText("Grow");

				_Owner->EnableStencil(true);
			}
			else
			{
				_Owner->EnableStencil(false);
			}
			break;
		}
	case EGrowSpotState::Growing:
		{
			_InvokingWidget->SetHUDInteractText("");
			_Owner->EnableStencil(false);
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (!_Owner->HeldItem && (Plant || Weapon))
			{
				// Set to "Grow"
				_InvokingWidget->SetHUDInteractText("Pick Up");
				_Owner->EnableStencil(true);
			}
			else
			{
				_Owner->EnableStencil(false);
			}
					
			break;
		}
	default:
		{
			_Owner->EnableStencil(false);
			break;
		}
	}
}

void AGrowSpot::SetPlant(APlant* _Plant, float _GrowTime)
{
	Plant = _Plant;
	GrowTimer = _GrowTime;
	GrowSpotState = EGrowSpotState::Growing;
		
	if (GrowTimer <= 0)
	{
		GrowTimer = 1.0f;
	}
}

void AGrowSpot::SetWeapon(AGrowableWeapon* _Weapon, float _GrowTime)
{
	Weapon = _Weapon;
	GrowTimer = _GrowTime;
	GrowSpotState = EGrowSpotState::Growing;
		
	if (GrowTimer <= 0)
	{
		GrowTimer = 1.0f;
	}
}


void AGrowSpot::Multi_SetPlantReadySparkle_Implementation(bool _bIsActive)
{
	if (PlantReadySparkle_NiagaraComponent)
	{
		if (_bIsActive)
		{
			if (!PlantReadySparkle_NiagaraComponent->IsActive())
			{
				PlantReadySparkle_NiagaraComponent->Activate();
			}
		}
		else
		{
			PlantReadySparkle_NiagaraComponent->Deactivate();
		}
	}
}

