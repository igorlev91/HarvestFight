

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
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/VFX/SquashAndStretch.h"

AGrowSpot::AGrowSpot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	ItemComponent->Mesh->SetIsReplicated(false);

	InterfaceType = EInterfaceType::GrowSpot;

	PlantReadyComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Attack Component"));
	PlantReadyComponent->SetupAttachment(RootComponent);
	PlantReadyComponent->bAutoActivate = false;

	SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));
}

void AGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGrowSpot, GrowTimer);
	DOREPLIFETIME(AGrowSpot, GrowTime);
	DOREPLIFETIME(AGrowSpot, Beehive);
	DOREPLIFETIME(AGrowSpot, GrowSpotState);
	DOREPLIFETIME(AGrowSpot, OwningPlayerName);
	DOREPLIFETIME(AGrowSpot, PlantReadyComponent);
	DOREPLIFETIME(AGrowSpot, bIsFertilised);
	DOREPLIFETIME(AGrowSpot, FertiliseInteractDelayTimer);
	DOREPLIFETIME(AGrowSpot, GrowingActor);
	DOREPLIFETIME(AGrowSpot, GrowingItemRef);
	DOREPLIFETIME(AGrowSpot, DefaultMaterial);
	DOREPLIFETIME(AGrowSpot, GoldMaterial);
}

bool AGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;
	
	if (_Player->PlayerName != OwningPlayerName)
		return false;

	APlayerController* Controller = _Player->GetPlayerController();
	if (!Controller)
		return false;

	ACharacter* Character = Controller->GetCharacter();
	if (!Character)
		return false;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(Character);
	if (!CastedCharacter)
		return false;

	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return true;
			if (Cast<AFertiliser>(CastedCharacter->HeldItem) && !bIsFertilised)
				return true;
			
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (Cast<AFertiliser>(CastedCharacter->HeldItem))
				return true;
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (Cast<AFertiliser>(CastedCharacter->HeldItem))
				return true;
		
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return false;
			
			if (ABeehive* SomeBeehive = Cast<ABeehive>(GrowingActor))
				return SomeBeehive->IsInteractable(_Player);

			return true;
		}
	default:
		{
			break;
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
				
			}
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (_Player->PlayerHUDRef)
				_Player->PlayerHUDRef->ClearPickupUI();
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (_Player->PlayerHUDRef)
				_Player->PlayerHUDRef->ClearPickupUI();
		}
	default:
		break;
	}
}

void AGrowSpot::PlantASeed(ASeed* _SeedToPlant)
{
	if (_SeedToPlant->SeedData->GrowTime > 0)
	{
		GrowTime = _SeedToPlant->SeedData->GrowTime;
		GrowTimer = _SeedToPlant->SeedData->GrowTime;
	}
	else
	{
		GrowTimer = 1.0f;
		GrowTime = 1.0f;
	}

	if (_SeedToPlant->SeedData->Type == EPickupDataType::WeaponData)
	{
		AGrowableWeapon* NewItem = GetWorld()->SpawnActor<AGrowableWeapon>(WeaponPrefab);
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->SeedData,EPickupActor::WeaponActor);
	}
	else if (_SeedToPlant->SeedData->Type == EPickupDataType::BeehiveData)
	{
		ABeehive* NewItem = GetWorld()->SpawnActor<ABeehive>(BeehivePrefab);
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->SeedData,EPickupActor::BeehiveActor);
		NewItem->ParentGrowSpot = this;
	}
	else
	{
		APickUpItem* NewItem = GetWorld()->SpawnActor<APickUpItem>(PlantPrefab);
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->SeedData, EPickupActor::PlantActor);
	}
	
	if (GrowingItemRef)
	{
		// Init random gold
		if (bIsFertilised && _SeedToPlant->SeedData->Type != EPickupDataType::BeehiveData)
		{
			MakePlantGold();
			bIsFertilised = false;
			UpdateMaterial();
		}
		else if (_SeedToPlant->SeedData->Type != EPickupDataType::BeehiveData)
		{
			GrowingItemRef->ItemComponent->bGold = false;
			int32 X = rand() % 100;
			//X = 0;
			if (X < GrowingItemRef->SeedData->ChanceOfGold)
			{
				MakePlantGold();
			}
		}
		
		GrowingItemRef->ItemComponent->Mesh->SetSimulatePhysics(false);
		GrowingItemRef->ItemComponent->Multi_DisableCollisionAndAttach();
	}
	
	_SeedToPlant->Destroy();
	
	GrowSpotState = EGrowSpotState::Growing;
}

void AGrowSpot::DestroyPlant()
{
	if (!GrowingActor)
		return;

	Multi_SetPlantReadySparkle(false);
	GrowingActor->Destroy();
	GrowSpotState = EGrowSpotState::Empty;
	GrowingActor = nullptr;
	GrowingItemRef = nullptr;
	
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();
	
	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	
	if (PlantReadyComponent)
	{
		PlantReadyComponent->SetAsset(PlantReadySystem);
	}
	
	// Server only below
	if (!HasAuthority())
		return;
	
	if (RiseCurve)
	{
		StartZHeight = GetActorLocation().Z;
		FOnTimelineFloat FloatEvent{};
		FloatEvent.BindDynamic(this, &AGrowSpot::RiseTimelineUpdate);
		RiseTimeline.AddInterpFloat(RiseCurve, FloatEvent);
		RiseTimeline.SetPlayRate(2.0f);
		RiseTimeline.PlayFromStart();
	}
}

void AGrowSpot::GrowPlantOnTick(float _DeltaTime)
{
	if (FertiliseInteractDelayTimer > 0)
	{
		FertiliseInteractDelayTimer -= _DeltaTime;
	}

	if (GrowSpotState == EGrowSpotState::Growing)
	{
		ScalePlantOnTick();

		if (GrowTimer > 0)
		{
			GrowTimer -= _DeltaTime;

			if (GrowTimer <= 0)
			{
				GrowSpotState = EGrowSpotState::Grown;

				if (GrowingActor)
				{
					if (!Cast<ABeehive>(GrowingActor))
					{
						Multi_SetPlantReadySparkle(true);
					}
					else
					{
						Multi_SetPlantReadySparkle(false);
					}
				}
				else
				{
					Multi_SetPlantReadySparkle(true);
				}
			}
		}
	}
}

// Called every frame
void AGrowSpot::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
		
	if (!HasAuthority())
		return;

	RiseTimeline.TickTimeline(_DeltaTime);
	GrowPlantOnTick(_DeltaTime);
}

void AGrowSpot::Multi_MakePlantGold_Implementation()
{
	if (!GrowingItemRef)
		return;
	
	for (int i = 0; i < GrowingItemRef->SeedData->BabyGoldMaterials.Num(); i++)
	{
		GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->SeedData->BabyGoldMaterials[i]);
	}
}

void AGrowSpot::MandrakePickupNoise(APrototype2Character* _Player)
{
	if (!_Player->HeldItem)
	{
		return;
	}

	if (!_Player->HeldItem->SeedData->BabyMesh)
	{
		return;
	}

	if (_Player->HeldItem->SeedData->Name.Compare("Mandrake"))
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

void AGrowSpot::ScalePlantOnTick() const
{
	if (!GrowingActor)
		return;
		
	const FVector Scale = FMath::Lerp<FVector>(GrowingItemRef->SeedData->BabyScale, FVector(0.3f, 0.3f, 0.3f) , GrowTimer / GrowTime);
	const FVector Pos = FMath::Lerp<FVector>({GetActorLocation()}, GetActorLocation() + FVector::UpVector, GrowTimer / GrowTime);
	GrowingItemRef->ItemComponent->Mesh->SetSimulatePhysics(false);
	GrowingItemRef->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrowingItemRef->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GrowingItemRef->ItemComponent->Mesh->SetWorldScale3D(Scale);
	GrowingActor->SetActorLocation(Pos);
}

void AGrowSpot::MakePlantGold()
{
	if (!GrowingItemRef)
		return;
	
	GrowingItemRef->ItemComponent->bGold = true;
	Multi_MakePlantGold();
}

void AGrowSpot::SetPlantReadySparkle(bool _bIsActive)
{
	if (PlantReadyComponent)
	{
		if (_bIsActive)
		{
			if (!PlantReadyComponent->IsActive())
			{
				PlantReadyComponent->Activate();
			}
		}
		else
		{
			PlantReadyComponent->Deactivate();
		}
	}
}

void AGrowSpot::UpdateMaterial()
{
	Multi_UpdateMaterial();
}

void AGrowSpot::Multi_UpdateMaterial_Implementation()
{
	if (!GoldMaterial || !DefaultMaterial)
		return;
	if (bIsFertilised)
		ItemComponent->Mesh->SetMaterial(0, GoldMaterial);
	else
		ItemComponent->Mesh->SetMaterial(0, DefaultMaterial);
}

void AGrowSpot::RiseTimelineUpdate(float _Delta)
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = FMath::Lerp(StartZHeight - 70.0f, StartZHeight, _Delta);
	SetActorLocation(NewLocation);
}


void AGrowSpot::Interact(APrototype2Character* _Player)
{
	if (!_Player->PlayerStateRef)
		return;
	
	switch(GrowSpotState)
	{
		case EGrowSpotState::Empty:
			{
				if (!_Player->HeldItem)
					break;
				
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					bIsFertilised = true;
					UpdateMaterial();
					_Player->DropItem();
					_Player->EnableStencil(false);
					Fertiliser->Destroy();
					break;
				}
				else if (ASeed* Seed = Cast<ASeed>(_Player->HeldItem))
				{
					if (!_Player->HeldItem->SeedData)
					{
						UE_LOG(LogTemp, Warning, TEXT("Could not plant seed data asset"));
						return;
					}
					
					// Audio
					if (_Player->PlantCue)
					{
						_Player->PlaySoundAtLocation(GetActorLocation(), _Player->PlantCue);
					}

					PlantASeed(Seed);

					//_Player->SocketWeapon(FName("Base-HumanWeapon"));

					_Player->DropItem();
					_Player->EnableStencil(false);
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				if (!_Player->HeldItem)
					break;
				
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					if (GrowingItemRef->SeedData->Type == EPickupDataType::BeehiveData)
						break;
					
					if (!GrowingItemRef->ItemComponent->bGold)
					{
						MakePlantGold();
						Fertiliser->Destroy();
						bIsFertilised = false;
						UpdateMaterial();
						_Player->DropItem();
						_Player->EnableStencil(false);
					}
				}
				else
				{
					_Player->DropItem();
					_Player->EnableStencil(false);
				}
				break;
			}
		case EGrowSpotState::Grown:
			{
				if (_Player->HeldItem)
				{
					if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
					{
						if (GrowingItemRef->SeedData->Type == EPickupDataType::BeehiveData)
						{
							_Player->DropItem();
							_Player->EnableStencil(false);
						}
						else if (!GrowingItemRef->ItemComponent->bGold)
						{
							MakePlantGold();
							Fertiliser->Destroy();
							FertiliseInteractDelayTimer = FertiliseInteractDelay;
							bIsFertilised = false;
							UpdateMaterial();
							_Player->DropItem();
							_Player->EnableStencil(false);
							break;
						}
					}
					else
					{
						_Player->DropItem();
						_Player->EnableStencil(false);
					}
				}

				if (GrowingItemRef && FertiliseInteractDelayTimer <= 0)
				{
					// causes crash atm
					//MandrakePickupNoise(_Player);
					
					// Plant

					switch(GrowingItemRef->SeedData->Type)
					{
					case EPickupDataType::PlantData:
						{
							APlant* SomePlant = Cast<APlant>(GrowingItemRef);
							SomePlant->bGrown = true;
							_Player->PickupItem(GrowingItemRef, EPickupActor::PlantActor);
							_Player->EnableStencil(false);

							break;
						}
					case EPickupDataType::WeaponData:
						{
							AGrowableWeapon* SomeWeapon = Cast<AGrowableWeapon>(GrowingItemRef);
							SomeWeapon->bGrown = true;
							_Player->PickupItem(GrowingItemRef, EPickupActor::WeaponActor);
							_Player->EnableStencil(false);
							
							break;
						}
					case EPickupDataType::BeehiveData:
						{
							ABeehive* SomeBeehive = Cast<ABeehive>(GrowingItemRef);
							SomeBeehive->bGrown = true;
							SomeBeehive->Interact(_Player);
							_Player->EnableStencil(false);
							
							if (SomeBeehive->HarvestedHoney < SomeBeehive->MaxHarvestableHoney)
							{
								return;
							}

							SomeBeehive->Destroy();
							break;
						}
					case EPickupDataType::FlowerData:
						{
							APlant* SomePlant = Cast<APlant>(GrowingItemRef);
							SomePlant->bGrown = true;
							_Player->PickupItem(GrowingItemRef, EPickupActor::PlantActor);
							_Player->EnableStencil(false);
							break;
						}
					default:
						{
							_Player->PickupItem(GrowingItemRef, EPickupActor::Default);
							_Player->EnableStencil(false);
							break;
						}

					}
					
					
					ItemComponent->Mesh->SetRenderCustomDepth(false);
					Multi_SetPlantReadySparkle(false);
				}

				GrowSpotState = EGrowSpotState::Empty;
				GrowingItemRef = nullptr;
				GrowingActor = nullptr;
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
			if(Cast<ASeed>(_Owner->HeldItem))
			{
				_InvokingWidget->SetHUDInteractText("Grow");

				_Owner->EnableStencil(true);
				return;
			}
			else if(Cast<AFertiliser>(_Owner->HeldItem))
			{
						
				_InvokingWidget->SetHUDInteractText("Fertilise");

				_Owner->EnableStencil(true);
				return;
			}
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (Cast<AFertiliser>(_Owner->HeldItem) && !GrowingItemRef->ItemComponent->bGold)
			{
				_InvokingWidget->SetHUDInteractText("Fertilise");

				_Owner->EnableStencil(true);
				return;
			}
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (Cast<AFertiliser>(_Owner->HeldItem) && !GrowingItemRef->ItemComponent->bGold)
			{
				// Fert
				_InvokingWidget->SetHUDInteractText("Fertilise");
				_Owner->EnableStencil(true);
				return;
			}
			else if (GrowingItemRef)
			{
				_InvokingWidget->SetHUDInteractText("Harvest");
				_Owner->EnableStencil(true);
				return;
			}
					
			break;
		}
	default:
		break;
	}

	_Owner->EnableStencil(false);
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
	if (PlantReadyComponent)
	{
		if (_bIsActive)
		{
			if (!PlantReadyComponent->IsActive())
			{
				PlantReadyComponent->Activate(true);
			}
		}
		else
		{
			PlantReadyComponent->Deactivate();
		}
	}
}
