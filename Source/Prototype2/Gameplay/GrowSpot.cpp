

#include "GrowSpot.h"
#include "GrowSpot.h"
#include "Prototype2/Pickups/Plant.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/Pickups/GrowableWeapon.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Prototype2/Pickups/Weapon.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/DataAssets/PlantData.h"

AGrowSpot::AGrowSpot()
{
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
			if (auto Casted = Cast<APrototype2Character>(Character))
			{
				if (_Player->Player_ID == Player_ID)
				{
					switch(GrowSpotState)
					{
					case EGrowSpotState::Empty:
						{
							if (Cast<ASeed>(Casted->HeldItem))
							{
								return true;
							}
							if (Cast<AFertiliser>(Casted->HeldItem) && !bIsFertilised)
							{
								return true;
							}
							break;
						}
					case EGrowSpotState::Growing:
						{
							if (Cast<AFertiliser>(Casted->HeldItem) && !Weapon)
								return true;
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
	const FVector Scale = FMath::Lerp<FVector>(_TargetScale, FVector(0.3, 0.3, 0.3), GrowTimer / GrowTime);
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

	if (FertiliseInteractDelayTime > 0)
	{
		FertiliseInteractDelayTime -= _DeltaTime;
	}
	
	if (Plant)
	{
		ScalePlant(Plant, Plant->PlantData->PlantScale, 0.0f);
	}
	
	if (Weapon)
	{
		ScalePlant(Weapon, {2.0f, 2.0f, 2.0f}, 0.0f);
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

void AGrowSpot::MakePlantGold()
{
	if (Plant->PlantData->GoldMaterial1)
	{
		Plant->ItemComponent->Mesh->SetMaterial(0, Plant->PlantData->GoldMaterial1);
	}
	if (Plant->PlantData->GoldMaterial2)
	{
		Plant->ItemComponent->Mesh->SetMaterial(1, Plant->PlantData->GoldMaterial2);
	}
	if (Plant->PlantData->GoldMaterial3)
	{
		Plant->ItemComponent->Mesh->SetMaterial(2, Plant->PlantData->GoldMaterial3);
	}

	Plant->ItemComponent->bGold = true;
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
				UE_LOG(LogTemp, Warning, TEXT("Interacted with Empty"));
				if (bIsFertilised && Cast<AFertiliser>(_Player->HeldItem))
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
				
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					bIsFertilised = true;
					Fertiliser->Destroy();

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
				if (auto Seed = Cast<ASeed>(_Player->HeldItem))
				{
					UE_LOG(LogTemp, Warning, TEXT("Attempted to plant something!"));
										
					// Planting seed from PlantData Data Asset
					// If no Data Asset, return.					
					if (!_Player->HeldItem->PlantData && !_Player->HeldItem->WeaponData)
					{
						UE_LOG(LogTemp, Warning, TEXT("Could not plant seed, no data asset"));
						return;
					}
					
					// Audio
					if (_Player->PlantCue)
					{
						_Player->PlaySoundAtLocation(GetActorLocation(), _Player->PlantCue);
					}

					if (_Player->HeldItem->DataAssetPickupType == EPickupDataType::WeaponData)
					{
						PlantWeapon(_Player);
					}
					else if (_Player->HeldItem->DataAssetPickupType == EPickupDataType::PlantData)
					{
						PlantPlant(_Player);
					}
					
					
					/*// Planting seed from old way
					// // seed has an assigned plant
					// if (!Seed->PlantToGrow)
					// {
					// 	return;
					// }
					//
					// // Audio
					// if (_Player->PlantCue)
					// {
					// 	_Player->PlaySoundAtLocation(GetActorLocation(), _Player->PlantCue);
					// }
					//
					// // Set grow time
					// if (Seed->GrowTime > 0)
					// {
					// 	GrowTimer = Seed->GrowTime;
					// }
					// else if (Seed->ItemComponent->GrowTime > 0)
					// {
					// 	// deprecated soon
					// 	GrowTimer = Seed->ItemComponent->GrowTime;
					// }
					// else
					// {
					// 	GrowTimer = 1.0f;
					// }
					// 		
					// if (Seed->bIsWeapon)
					// {
					// 	Weapon = GetWorld()->SpawnActor<AGrowableWeapon>(Seed->PlantToGrow);
					// 	SetWeapon(Weapon, GrowTime);
					// }
					// else
					// {
					// 	Plant = GetWorld()->SpawnActor<APlant>(Seed->PlantToGrow);
					// 	SetPlant(Plant, GrowTime);
					// }
					//
					// if (bIsFertilised && Plant)
					// {
					// 	Plant->MakeGold();
					// 	bIsFertilised = false;
					// }
					// 		
					// Multi_Plant();
					// 	
					// if (Seed)
					// 	Seed->Destroy();
					//
					// _Player->HeldItem = nullptr;
					//
					// if (_Player->PlayerHUDRef)
					// {
					// 	_Player->PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
					// 	_Player->PlayerHUDRef->SetHUDInteractText("");
					// }
					// _Player->EnableStencil(false);
					//
					// Multi_UpdateState(EGrowSpotState::Growing);
					// GrowSpotState = EGrowSpotState::Growing;*/
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				UE_LOG(LogTemp, Warning, TEXT("Interacted with Growing"));
				if (!_Player->HeldItem)
					break;
				if (Weapon)
					break;
				if (Beehive)
					break;
				
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					if (Plant && !Plant->ItemComponent->bGold)
					{
						MakePlantGold();
						Fertiliser->Destroy();
					}
				}
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
		case EGrowSpotState::Grown:
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempted to Harvest something!"));
						
				if (_Player->HeldItem && !Beehive)
				{
					if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
					{
						if (Plant && !Plant->ItemComponent->bGold)
						{
							MakePlantGold();
							Fertiliser->Destroy();
							FertiliseInteractDelayTime = FertiliseInteractDelay;
						}
					}
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
				}

				if (Beehive)
				{
					// do beehive stuff
					break;
				}

				if (Weapon)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tis a weapon!"));
					Weapon->bGrown = true;
							
					_Player->Server_PickupItem(Weapon->ItemComponent, Weapon);
					_Player->WeaponCurrentDurability = _Player->GetWeaponData()->Durability;
					
					//// Change the weapon UI for this player
					_Player->GetPlayerHUD()->UpdateWeaponUI(EPickup::Weapon);
					
					Weapon->Destroy();
					Weapon = nullptr;
				
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
				
				if (Plant && FertiliseInteractDelayTime <= 0)
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
			else if(_Owner->HeldItem && Cast<AFertiliser>(_Owner->HeldItem))
			{
						
				_InvokingWidget->SetHUDInteractText("Fertilise");

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
			if(_Owner->HeldItem && Cast<AFertiliser>(_Owner->HeldItem) && !Plant->ItemComponent->bGold && Plant && !Weapon)
			{
						
				_InvokingWidget->SetHUDInteractText("Fertilise");

				_Owner->EnableStencil(true);
			}
			else
			{
				_Owner->EnableStencil(false);
			}
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (_Owner->HeldItem && Cast<AFertiliser>(_Owner->HeldItem) && Plant && Plant->ItemComponent->bGold)
			{
				_InvokingWidget->SetHUDInteractText("Pick Up");
				_Owner->EnableStencil(true);
			}
			else if(_Owner->HeldItem && Cast<AFertiliser>(_Owner->HeldItem) && Plant && !Weapon)
			{
				_InvokingWidget->SetHUDInteractText("Fertilise");

				_Owner->EnableStencil(true);
			}
			else if (_Owner->HeldItem && (Plant || Weapon) && !Beehive)
			{
				// Set to "Grow"
				_InvokingWidget->SetHUDInteractText("Pick Up");
				_Owner->EnableStencil(true);
			}
			else if (!_Owner->HeldItem && (Plant || Weapon) && !Beehive)
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

void AGrowSpot::PlantWeapon(APrototype2Character* _Player)
{
	if (!WeaponPrefab)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponPrefab not set."));
		return;
	}
	// Set grow time
	if (_Player->HeldItem->WeaponData->GrowTime > 0)
	{
		GrowTime = _Player->HeldItem->WeaponData->GrowTime;
	}
	else
	{
		GrowTime = 1.0f;
	}

	Weapon = GetWorld()->SpawnActor<AGrowableWeapon>(WeaponPrefab);
	Weapon->SetWeaponData(_Player->HeldItem->WeaponData);
	Weapon->ItemComponent->bGold = false;
	SetWeapon(Weapon, GrowTime);

	// Todo: Dawn we need this here?
	//if (bIsFertilised && Plant && !Plant->ItemComponent->bGold)
	//{
	//	MakePlantGold();
	//	bIsFertilised = false;
	//}
							
	Multi_Plant();

	_Player->HeldItem->Destroy();
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

void AGrowSpot::PlantPlant(APrototype2Character* _Player)
{
	
	// Set grow time
	if (_Player->HeldItem->PlantData->GrowTime > 0)
	{
		GrowTime = _Player->HeldItem->PlantData->GrowTime;
	}
	else
	{
		GrowTime = 1.0f;
	}

	Plant = GetWorld()->SpawnActor<APlant>(PlantPrefab);
	// doesnt properly check beehive yet
	if (auto BeehiveCast = Cast<ABeehive>(Plant))
	{
		SetBeehive(BeehiveCast, BeehiveCast->PlantData->GrowTime);
	}
	else
	{
		Plant->SetPlantData(_Player->HeldItem->PlantData);
		Plant->ItemComponent->bGold = false;
		int32 X = rand() % 100;
		if (X < Plant->PlantData->ChanceOfGold)
		{
			MakePlantGold();
		}
		SetPlant(Plant, GrowTime);

		// Todo: Dawn we need this here?
		if (bIsFertilised && Plant && !Plant->ItemComponent->bGold)
		{
			MakePlantGold();
			bIsFertilised = false;
		}
	}
							
	Multi_Plant();

	_Player->HeldItem->Destroy();
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

void AGrowSpot::SetBeehive(ABeehive* _Beehive, float _GrowTime)
{
	Beehive = _Beehive;
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

void AGrowSpot::TemporaryPlantWeapon(ASeed* _WeaponSeed, APrototype2Character* _Player)
{
	//Planting seed from old way
	// seed has an assigned plant
	if (!_WeaponSeed->PlantToGrow)
	{
		return;
	}
	
	// Audio
	if (_Player->PlantCue)
	{
		_Player->PlaySoundAtLocation(GetActorLocation(), _Player->PlantCue);
	}
	
	// Set grow time
	if (_WeaponSeed->GrowTime > 0)
	{
		GrowTime = _WeaponSeed->GrowTime;
	}
	else if (_WeaponSeed->ItemComponent->GrowTime > 0)
	{
		// deprecated soon
		GrowTime = _WeaponSeed->ItemComponent->GrowTime;
	}
	else
	{
		GrowTimer = 1.0f;
	}
			
	if (_WeaponSeed->bIsWeapon)
	{
		Weapon = GetWorld()->SpawnActor<AGrowableWeapon>(_WeaponSeed->PlantToGrow);
		SetWeapon(Weapon, GrowTime);
	}
	else
	{
		Plant = GetWorld()->SpawnActor<APlant>(_WeaponSeed->PlantToGrow);
		SetPlant(Plant, GrowTime);
	}
	
	if (bIsFertilised && Plant)
	{
		Plant->MakeGold();
		bIsFertilised = false;
	}
			
	Multi_Plant();
		
	if (_WeaponSeed)
		_WeaponSeed->Destroy();
	
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