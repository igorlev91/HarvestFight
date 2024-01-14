

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
#include "RadialPlot.h"
#include "Prototype2/Pickups/Weapon.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/DataAssets/PlantData.h"
#include "Prototype2/Pickups/PlantSeed.h"
#include "Prototype2/VFX/SquashAndStretch.h"

AGrowSpot::AGrowSpot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	ItemComponent->Mesh->SetIsReplicated(false);

	InteractSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	InteractSystem->SetupAttachment(RootComponent);

	InterfaceType = EInterfaceType::GrowSpot;

	PlantReadySparkle_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Attack Component"));
	PlantReadySparkle_NiagaraComponent->SetupAttachment(RootComponent);

	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void AGrowSpot::SetRadialReferance(ARadialPlot* _RadialPlot)
{
	//RadialPlot = _RadialPlot;
}

void AGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGrowSpot, GrowTimer);
	DOREPLIFETIME(AGrowSpot, GrowTime);
	DOREPLIFETIME(AGrowSpot, Plant);
	DOREPLIFETIME(AGrowSpot, Weapon);
	DOREPLIFETIME(AGrowSpot, Beehive);
	DOREPLIFETIME(AGrowSpot, GrowSpotState);
	DOREPLIFETIME(AGrowSpot, Player_ID);
	DOREPLIFETIME(AGrowSpot, PlantReadySparkle_NiagaraComponent);

	DOREPLIFETIME(AGrowSpot, bIsFertilised);
	DOREPLIFETIME(AGrowSpot, FertiliseInteractDelayTime);
}

bool AGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;
	
	if (_Player->Player_ID != Player_ID)
		return false;
	
	if (auto Controller = _Player->GetPlayerController())
	{
		if (auto Character = Controller->GetCharacter())
		{
			if (auto Casted = Cast<APrototype2Character>(Character))
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
	

	return false;
}

void AGrowSpot::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);

	if (!_Player->PlayerStateRef)
		return;
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(_Player->HeldItem))
			{
				if (_Player->PlayerHUDRef)
					_Player->PlayerHUDRef->ClearPickupUI();
				//_Player->UpdateDecalDirection(false, false);
			}
			break;
		}
	case EGrowSpotState::Growing:
		{
			break;
		}
	case EGrowSpotState::Grown:
		{
			//if (Plant)
			//	_Player->UpdateDecalDirection(true, true);
			//else
			//	_Player->UpdateDecalDirection(false);

			if (Plant)
				Plant->SSComponent->Boing();
			if (Weapon)
				Weapon->SSComponent->Boing();
		}
	default:
		break;
	}
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();

	SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), 0));

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

	if (HasAuthority() && RiseCurve)
	{
		StartZHeight = GetActorLocation().Z;
		FOnTimelineFloat FloatEvent{};
		FloatEvent.BindDynamic(this, &AGrowSpot::RiseTimelineUpdate);
		RiseTimeline.AddInterpFloat(RiseCurve, FloatEvent);
		RiseTimeline.SetPlayRate(2.0f);
		RiseTimeline.PlayFromStart();
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
	if (FertiliseInteractDelayTime > 0)
	{
		FertiliseInteractDelayTime -= _DeltaTime;
	}
	
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
				//Plant->SSComponent->Enable();
			}
			if (Weapon)
			{
				Weapon->bGrown = true;
				//Weapon->SSComponent->Enable();
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
	if (Weapon)
	{
		_Plant->SetActorRotation(FRotator(90,0,0));
	}
	else
	{
		_Plant->SetActorRotation(FRotator(0,0,0));
	}
}


// Called every frame
void AGrowSpot::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	
	if (Beehive)
	{
		ScalePlant(Beehive, Beehive->PlantData->PlantScale, 0.0f);
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
	{
		RiseTimeline.TickTimeline(_DeltaTime);
		GrowPlantOnTick(_DeltaTime);
	}
	
	// VFX
	//if (GrowSpotState == EGrowSpotState::Grown)
	//{
	//	Multi_SetPlantReadySparkle_Implementation(true);
	//}
	//else
	//{
	//	Multi_SetPlantReadySparkle_Implementation(false);
	//}

	
}

void AGrowSpot::Multi_MakePlantGold_Implementation()
{
	if (!Plant)
		return;
	
	if (!Plant->PlantData)
	{
		return;
	}

	Plant->ItemComponent->bGold = true;
	
	for (int i = 0; i < Plant->PlantData->GoldMaterials.Num(); i++)
	{
		Plant->ItemComponent->Mesh->SetMaterial(i, Plant->PlantData->GoldMaterials[i]);
	}
}

void AGrowSpot::Multi_SetFertilised_Implementation()
{
	bIsFertilised = true;
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

	if (!_Player->HeldItem->PlantData)
	{
		return;
	}

	if (_Player->HeldItem->PlantData->Name.Compare("Mandrake"))
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
	Multi_MakePlantGold();
}

void AGrowSpot::RiseTimelineUpdate(float _Delta)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = FMath::Lerp(StartZHeight - 70.0f, StartZHeight, _Delta);
	SetActorLocation(NewLocation);
}


void AGrowSpot::Interact(APrototype2Character* _Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempted to interact with the grow spot"));

	if (!_Player->PlayerStateRef)
		return;
	
	switch(GrowSpotState)
		{
		case EGrowSpotState::Empty:
			{
				UE_LOG(LogTemp, Warning, TEXT("Interacted with Empty"));
				if (bIsFertilised && Cast<AFertiliser>(_Player->HeldItem))
				{
					UE_LOG(LogTemp, Warning, TEXT("But dropped it..."));
					_Player->Server_DropItem();
					_Player->EnableStencil(false);
					break;
				}
				
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					Multi_SetFertilised();
					Fertiliser->Destroy();

					UE_LOG(LogTemp, Warning, TEXT("But dropped it..."));
					_Player->Server_DropItem();
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
					else
					{
						PlantPlant(_Player);
					}

					_Player->SocketWeapon(FName("Base-HumanWeapon"));
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
						bIsFertilised = false;
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("But dropped it..."));
				_Player->Server_DropItem();
				_Player->EnableStencil(false);
				break;
			}
		case EGrowSpotState::Grown:
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempted to Harvest something!"));

				if (Beehive)
				{
					//RadialPlot->UpdateBeehiveFlowers();
				}
						
				if (_Player->HeldItem && !Beehive)
				{
					if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
					{
						if (Plant && !Plant->ItemComponent->bGold)
						{
							MakePlantGold();
							Fertiliser->Destroy();
							FertiliseInteractDelayTime = FertiliseInteractDelay;
							bIsFertilised = false;
						}
					}
					UE_LOG(LogTemp, Warning, TEXT("But dropped it..."));
					_Player->Server_DropItem();
					_Player->EnableStencil(false);
				}

				if (Beehive)
				{
					// do beehive stuff
					Beehive->Interact(_Player);
					break;
				}

				if (Weapon)
				{
					UE_LOG(LogTemp, Warning, TEXT("Tis a weapon!"));
					Weapon->bGrown = true;
					//Weapon->SSComponent->Disable();
							
					_Player->PickupItem(Weapon);

					Weapon->Destroy();
					Weapon = nullptr;

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
					//Plant->SSComponent->Disable();

					Plant->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					
					_Player->PickupItem(Plant);

					// Special sound for mandrake when picked up because its special
					MandrakePickupNoise(_Player);
					
					Plant = nullptr;

					_Player->EnableStencil(false);
					ItemComponent->Mesh->SetRenderCustomDepth(false);

					UE_LOG(LogTemp, Warning, TEXT("Empty the plot."));
					Multi_UpdateState(EGrowSpotState::Empty);
					GrowSpotState = EGrowSpotState::Empty;
				}

				if (Plant)
				{
					if (Plant->PlantData->PickupType == EPickupDataType::FlowerData)
					{
						//RadialPlot->UpdateBeehiveFlowers();
					}	
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
			if(_Owner->HeldItem && Cast<AFertiliser>(_Owner->HeldItem) && !Plant->ItemComponent->bGold && Plant && !Weapon && !Beehive)
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
			else if(_Owner->HeldItem && Cast<AFertiliser>(_Owner->HeldItem) && Plant && !Weapon && !Beehive)
			{
				_InvokingWidget->SetHUDInteractText("Fertilise");

				_Owner->EnableStencil(true);
			}
			else if (Beehive && Beehive->bIsReadyToCollect)
			{
				_InvokingWidget->SetHUDInteractText("Pick Up");
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
							
	Multi_Plant();

	_Player->HeldItem->Destroy();
	_Player->HeldItem = nullptr;

	if (_Player->PlayerHUDRef)
	{
		_Player->PlayerHUDRef->ClearPickupUI();
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

	if (_Player->HeldItem->PlantData->PickupType == EPickupDataType::BeehiveData)
	{
		Beehive = GetWorld()->SpawnActor<ABeehive>(BeehivePrefab);
	}
	else
	{
		Plant = GetWorld()->SpawnActor<APlant>(PlantPrefab);
	}
	// doesnt properly check beehive yet
	if (Beehive)
	{
		Beehive->SetPlantData(_Player->HeldItem->PlantData);
		SetBeehive(Beehive, Beehive->PlantData->GrowTime);
	}
	else if (Plant)
	{
		Plant->SetPlantData(_Player->HeldItem->PlantData);
		Plant->ItemComponent->bGold = false;
		int32 X = rand() % 100;
		if (X < Plant->PlantData->ChanceOfGold)
		{
			MakePlantGold();
		}
		SetPlant(Plant, GrowTime);

		if (bIsFertilised)
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
		_Player->PlayerHUDRef->ClearPickupUI();
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
