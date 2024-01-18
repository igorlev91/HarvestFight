

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
	DOREPLIFETIME(AGrowSpot, OwningPlayerID);
	DOREPLIFETIME(AGrowSpot, PlantReadyComponent);
	DOREPLIFETIME(AGrowSpot, bIsFertilised);
	DOREPLIFETIME(AGrowSpot, FertiliseInteractDelayTimer);
	DOREPLIFETIME(AGrowSpot, GrowingActor);
	DOREPLIFETIME(AGrowSpot, GrowingItemRef);
}

bool AGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (!_Player)
		return false;
	
	if (_Player->Player_ID != OwningPlayerID)
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
			{
				return true;
			}
			if (Cast<AFertiliser>(CastedCharacter->HeldItem) && !bIsFertilised)
			{
				return true;
			}
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
			if (GrowingItemRef)
				return true;
		}
	default:
		break;
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
			
			if (GrowingItemRef)
			{
				if (_Player->PlayerHUDRef)
				{
					if (GrowingItemRef->PickupActor == EPickupActor::WeaponActor)
					{
						_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->WeaponData->WeaponIcon);
					}
					else
					{
						_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->PlantData->PlantIcon);
					}
				}
			}
		}
	default:
		break;
	}
}

void AGrowSpot::PlantASeed(ASeed* _SeedToPlant)
{
	if (_SeedToPlant->PlantData->GrowTime > 0)
	{
		GrowTime = _SeedToPlant->PlantData->GrowTime;
		GrowTimer = _SeedToPlant->PlantData->GrowTime;
	}
	else
	{
		GrowTimer = 1.0f;
		GrowTime = 1.0f;
	}

	APickUpItem* NewItem = GetWorld()->SpawnActor<APickUpItem>(PlantPrefab);
	GrowingActor = NewItem;
	GrowingItemRef = NewItem;
	
	if (GrowingItemRef)
	{
		GrowingItemRef->SetPlantData(_SeedToPlant->PlantData);

		// Init random gold
		GrowingItemRef->ItemComponent->bGold = false;
		int32 X = rand() % 100;
		if (X < GrowingItemRef->PlantData->ChanceOfGold)
		{
			MakePlantGold();
		}
		
		if (bIsFertilised)
		{
			MakePlantGold();
			bIsFertilised = false;
		}
		GrowingItemRef->ItemComponent->Mesh->SetSimulatePhysics(false);
		GrowingItemRef->ItemComponent->Multi_DisableCollisionAndAttach();
	}
	
	_SeedToPlant->Destroy();
	
	GrowSpotState = EGrowSpotState::Growing;
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();
	
	ItemComponent->Mesh->SetSimulatePhysics(false);
	ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
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

				Multi_SetPlantReadySparkle(true);
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
	
	for (int i = 0; i < GrowingItemRef->PlantData->GoldMaterials.Num(); i++)
	{
		GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->PlantData->GoldMaterials[i]);
	}
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

void AGrowSpot::ScalePlantOnTick() const
{
	if (!GrowingActor)
		return;
		
	const FVector Scale = FMath::Lerp<FVector>(GrowingItemRef->PlantData->PlantScale, FVector(0.3f, 0.3f, 0.3f) , GrowTimer / GrowTime);
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
				if (!_Player->HeldItem)
					break;
				
				UE_LOG(LogTemp, Warning, TEXT("Interacted with Empty"));
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					bIsFertilised = true;
					_Player->Server_DropItem();
					_Player->EnableStencil(false);
					Fertiliser->Destroy();
					break;
				}
				else if (ASeed* Seed = Cast<ASeed>(_Player->HeldItem))
				{
					UE_LOG(LogTemp, Warning, TEXT("Attempted to plant something!"));
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

					PlantASeed(Seed);

					//_Player->SocketWeapon(FName("Base-HumanWeapon"));

					_Player->Server_DropItem();
					_Player->EnableStencil(false);
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				UE_LOG(LogTemp, Warning, TEXT("Interacted with something growing"));
				if (!_Player->HeldItem)
					break;
				
				if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
				{
					if (!GrowingItemRef->ItemComponent->bGold)
					{
						MakePlantGold();
						Fertiliser->Destroy();
						bIsFertilised = false;
						_Player->Server_DropItem();
						_Player->EnableStencil(false);
					}
				}
				else
				{
					_Player->Server_DropItem();
					_Player->EnableStencil(false);
				}
				break;
			}
		case EGrowSpotState::Grown:
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempted to Harvest something!"));
						
				if (_Player->HeldItem)
				{
					if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
					{
						if (!GrowingItemRef->ItemComponent->bGold)
						{
							MakePlantGold();
							Fertiliser->Destroy();
							FertiliseInteractDelayTimer = FertiliseInteractDelay;
							bIsFertilised = false;
							_Player->Server_DropItem();
							_Player->EnableStencil(false);
						}
					}
					else
					{
						_Player->Server_DropItem();
						_Player->EnableStencil(false);
					}
				}

				if (GrowingItemRef && FertiliseInteractDelayTimer <= 0)
				{
					MandrakePickupNoise(_Player);
					// Plant
					if (APlant* SomePlant = Cast<APlant>(GrowingItemRef))
					{
						SomePlant->bGrown = true;
					}
					_Player->PickupItem(GrowingItemRef);
					_Player->EnableStencil(false);
					ItemComponent->Mesh->SetRenderCustomDepth(false);
					Multi_SetPlantReadySparkle(false);
				}
				
				if (Beehive)
				{
					Beehive->Interact(_Player);

					if (Beehive->HarvestedHoney < Beehive->MaxHarvestableHoney)
					{
						break;
					}

					Beehive->Destroy();
					Beehive = nullptr;

					_Player->EnableStencil(false);
					ItemComponent->Mesh->SetRenderCustomDepth(false);
					GrowSpotState = EGrowSpotState::Empty;
					break;
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
			if (!_Owner->HeldItem)
				break;
			
			if(Cast<ASeed>(_Owner->HeldItem))
			{
				_InvokingWidget->SetHUDInteractText("Grow");

				_Owner->EnableStencil(true);
			}
			else if(Cast<AFertiliser>(_Owner->HeldItem))
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
			if(!_Owner->HeldItem)
				return;
			if (Cast<AFertiliser>(_Owner->HeldItem) && !GrowingItemRef->ItemComponent->bGold)
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
			if (Cast<AFertiliser>(_Owner->HeldItem))
			{
				_InvokingWidget->SetHUDInteractText("Fertilise");
				_Owner->EnableStencil(true);
			}
			else if (Beehive && Beehive->bIsReadyToCollect)
			{
				_InvokingWidget->SetHUDInteractText("Pick Up");
				_Owner->EnableStencil(true);
			}
			else if(!_Owner->HeldItem)
			{
				// Set to "Grow"
				_InvokingWidget->SetHUDInteractText("Harvest");
				_Owner->EnableStencil(true);
			}
			else 
			{
				_Owner->EnableStencil(false);
			}
					
			break;
		}
	default:
		break;
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
