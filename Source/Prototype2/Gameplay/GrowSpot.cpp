

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
#include "Prototype2/DataAssets/ConcreteBagData.h"
#include "Prototype2/DataAssets/FertiliserData.h"
#include "Prototype2/DataAssets/GrowSpotData.h"
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
	
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	//SSComponent = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash Amd Stretch SComponent"));
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
	DOREPLIFETIME(AGrowSpot, OwningPlayerColor);
	DOREPLIFETIME(AGrowSpot, ConcretedHealth);
	DOREPLIFETIME(AGrowSpot, ConcretedDamageTimer);
}

bool AGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (ConcretedHealth > 0)
		return false;
	
	if (!_Player)
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

	//
	if (CastedCharacter->HeldItem &&
	CastedCharacter->HeldItem->SeedData &&
	CastedCharacter->HeldItem->SeedData->FertiliserData &&
	CastedCharacter->HeldItem->SeedData->FertiliserData->bConcrete)
	{
		if (_Player->Details.Colour == OwningPlayerColor)
			return false;

		return true;
	}

	// Todo: only delete when finished implementing stealing
	//if (_Player->Details.Colour != OwningPlayerColor)
	//	return false;
	
	return IsInteractable_Unprotected(_Player, true);
}

bool AGrowSpot::IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete)
{
	if (_LookOutForConcrete && ConcretedHealth > 0)
		return false;
	
	if (!_Player)
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
	
	// Check for player stealing
	if (_Player->Details.Colour != OwningPlayerColor
		&& bIsNormalGrowSpot)
	{
		return Stealing_IsInteractable_Unprotected(CastedCharacter);
	}
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return true;
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (!bIsFertilised)
					return true;
				else if (SomeFertilizer->SeedData->FertiliserData->bConcrete)
					return true;
				else
					return false;
			}
			
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (!bIsFertilised)
					return true;
				else if (SomeFertilizer->SeedData->FertiliserData->bConcrete)
					return true;
				else
					return false;
			}
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (!bIsFertilised)
					return true;
				else if (SomeFertilizer->SeedData->FertiliserData->bConcrete)
					return true;
				else
					return false;
			}
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return false;
			
			if (ABeehive* SomeBeehive = Cast<ABeehive>(GrowingActor))
				return SomeBeehive->IsInteractable(_Player);

			return true;
		}
	default:
		{
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (SomeFertilizer->SeedData->FertiliserData->bConcrete)
					return true;
			}
			break;
		}
	}

	return false;
}

void AGrowSpot::ClientInteract(APrototype2Character* _Player)
{
	if (!_Player->PlayerStateRef)
		return;
	
	if (_Player->PlayerStateRef->Details.Colour != OwningPlayerColor
		&& bIsNormalGrowSpot)
	{
		Stealing_ClientInteract(_Player);
		return;
	}
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(_Player->HeldItem))
			{
				if (_Player->PlayerHUDRef)
					_Player->PlayerHUDRef->ClearPickupUI();

				if (GrowingItemRef)
				{
					GrowingItemRef->Client_Pickup();
				}
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
			if (GrowingItemRef)
			{
				GrowingItemRef->Client_Pickup();
			}
			if (_Player->PlayerHUDRef)
			{
				if (bIsFertilised)
				{
					if (GrowingItemRef->SeedData)
					{
						if (GrowingItemRef->SeedData->PlantData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->PlantData->GoldPlantIcon);
						}
						else if (GrowingItemRef->SeedData->WeaponData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->WeaponData->GoldWeaponIcon);
						}
					}
				}
				else
				{
					if (GrowingItemRef->SeedData)
					{
						if (GrowingItemRef->SeedData->PlantData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->PlantData->PlantIcon);
						}
						else if (GrowingItemRef->SeedData->WeaponData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->WeaponData->WeaponIcon);
						}
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
	if (_SeedToPlant->SeedData->BabyGrowTime > 0)
	{
		GrowTime = _SeedToPlant->SeedData->BabyGrowTime;
		GrowTimer = _SeedToPlant->SeedData->BabyGrowTime;
	}
	else
	{
		GrowTimer = 1.0f;
		GrowTime = 1.0f;
	}

	if (_SeedToPlant->SeedData->BabyType == EPickupDataType::WeaponData)
	{
		AGrowableWeapon* NewItem = GetWorld()->SpawnActor<AGrowableWeapon>(WeaponPrefab);
		NewItem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewItem->SetActorRotation(FRotator{90.0f, 0.0f, 0.0f});
		NewItem->SetActorLocation(GetActorLocation() - (FVector::UpVector));
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->SeedData,EPickupActor::WeaponActor);
	}
	else if (_SeedToPlant->SeedData->BabyType == EPickupDataType::BeehiveData)
	{
		ABeehive* NewItem = GetWorld()->SpawnActor<ABeehive>(BeehivePrefab);
		NewItem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewItem->SetActorLocation(GetActorLocation() + (FVector::UpVector * 5.0f));
		NewItem->SetBeehiveLocation();
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->SeedData,EPickupActor::BeehiveActor);
		NewItem->ParentGrowSpot = this;
	}
	else
	{
		APickUpItem* NewItem = GetWorld()->SpawnActor<APickUpItem>(PlantPrefab);
		NewItem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewItem->SetActorLocation(GetActorLocation() + (FVector::UpVector * 5.0f));
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->SeedData, EPickupActor::PlantActor);
	}
	
	GrowingItemRef->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Init random gold
	if (bIsFertilised)
	{
		MakePlantGold();
	}
	else if (_SeedToPlant->SeedData->BabyType != EPickupDataType::BeehiveData)
	{
		GrowingItemRef->ItemComponent->bGold = false;
		int32 X = rand() % 100;
		//X = 0;
		if (X < GrowingItemRef->SeedData->ChanceOfGold)
		{
			bIsFertilised = true;
			MakePlantGold();
		}
	}
		
	GrowingItemRef->ItemComponent->Mesh->SetSimulatePhysics(false);
	GrowingItemRef->ItemComponent->Multi_DisableCollisionAndAttach();
	
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

void AGrowSpot::CompleteGrowth()
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

void AGrowSpot::DegradeConcrete()
{
	if (HasAuthority())
	{
		if (ConcretedHealth > 0 && ConcretedDamageTimer <= 0)
		{
			ConcretedDamageTimer = ConcretedDamageInterval;
			ConcretedHealth--;
			if (ConcretedHealth <= 0)
			{
				ConcretedHealth = 0;
				Multi_BrakePlantConcrete();
			}
		}
	}
	else
	{
			Server_DegradeConcrete();
	}
}

void AGrowSpot::Server_DegradeConcrete_Implementation()
{
	if (ConcretedHealth > 0 && ConcretedDamageTimer <= 0)
	{
		ConcretedDamageTimer = ConcretedDamageInterval;
		ConcretedHealth--;
		if (ConcretedHealth <= 0)
		{
			ConcretedHealth = 0;
			Multi_BrakePlantConcrete();
		}
	}
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();

	if (GrowSpotData)
	{
		ItemComponent->Mesh->SetStaticMesh(GrowSpotData->Mesh);
		ItemComponent->Mesh->SetWorldScale3D(GrowSpotData->DesiredScale);
	}

	//if (HasAuthority())
	//	SSComponent->SetMeshToStretch(ItemComponent->Mesh);
	
	if (ItemComponent->Mesh)
	{
		ItemComponent->Mesh->SetIsReplicated(false);
		ItemComponent->Mesh->SetSimulatePhysics(false);
		ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
		ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	HitBox->SetupAttachment(ItemComponent->Mesh);
	
	if (PlantReadyComponent)
	{
		PlantReadyComponent->SetAsset(PlantReadySystem);
	}
	
	// Server only below
	if (!HasAuthority())
		return;

	ConcretedDamageTimer = ConcretedDamageInterval;
	
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
	

	if (GrowTimer > 0 && ConcretedHealth <= 0)
	{
		GrowTimer -= _DeltaTime;

		if (GrowTimer > 0)
			return;
		
		CompleteGrowth();
	}
}

// Called every frame
void AGrowSpot::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	ScalePlantOnTick();
		
	if (!HasAuthority())
		return;

	RiseTimeline.TickTimeline(_DeltaTime);
	GrowPlantOnTick(_DeltaTime);

	if (ConcretedDamageTimer > 0)
	{
		ConcretedDamageTimer -= _DeltaTime;
	}

	if (CurrentPlayerStealing)
	{
		if (CurrentPlayerStealing->bIsHoldingInteract == false)
		{
			// Talk to HUD through delegates
			CurrentPlayerStealing->Client_OnStoppedClaimingPlot();

			CurrentPlayerStealing = nullptr;
		}
	}
}

void AGrowSpot::Multi_MakePlantGold_Implementation()
{
	if (!GrowingItemRef)
		return;

	for (int i = 0; i < GrowingItemRef->ItemComponent->Mesh->GetNumMaterials(); i++)
	{
		if (GrowingItemRef->SeedData->BabyGoldMaterials.Num() > i)
			GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->SeedData->BabyGoldMaterials[i]);
		else if (GrowingItemRef->SeedData->BabyGoldMaterials.Num() > 0)
			GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->SeedData->BabyGoldMaterials[0]);
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
	if (!GrowingActor || !GrowingItemRef)
		return;

	if (GrowingItemRef->SeedData->BabyType == EPickupDataType::BeehiveData)
		return;
	
	const FVector Scale = FMath::Lerp<FVector>(GrowingItemRef->SeedData->BabyScale, FVector(0.2f, 0.2f, 0.2f) , GrowTimer / GrowTime);

	GrowingItemRef->ItemComponent->Mesh->SetWorldScale3D(Scale);
}

void AGrowSpot::MakePlantGold()
{
	if (!GrowingItemRef)
		return;
	
	GrowingItemRef->ItemComponent->bGold = true;
	if (HasAuthority())
	{
		Multi_MakePlantGold();
	}
	else
	{
		Server_MakePlantGold();
	}
	UpdateMaterial();
}

void AGrowSpot::Server_MakePlantGold_Implementation()
{
	Multi_MakePlantGold();
}

void AGrowSpot::Multi_MakePlantConcrete_Implementation()
{
	if (!GrowingItemRef)
		return;
	
	for (int i = 0; i < GrowingItemRef->ItemComponent->Mesh->GetNumMaterials(); i++)
	{
		GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, ConcreteMaterial);
	}
}

void AGrowSpot::Multi_BrakePlantConcrete_Implementation()
{
	ItemComponent->Mesh->SetMaterial(0, DefaultMaterial);
	
	if (!GrowingItemRef)
		return;
	
	for (int i = 0; i < GrowingItemRef->ItemComponent->Mesh->GetNumMaterials(); i++)
	{
		if (GrowingItemRef->SeedData->BabyMaterials.Num() > i)
			GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->SeedData->BabyMaterials[i]);
		else if (GrowingItemRef->SeedData->BabyMaterials.Num() > 0)
			GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->SeedData->BabyMaterials[0]);
	}
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
	if (ConcretedHealth > 0 && ConcreteMaterial)
		ItemComponent->Mesh->SetMaterial(0, ConcreteMaterial);
	else if (bIsFertilised && GoldMaterial)
		ItemComponent->Mesh->SetMaterial(0, GoldMaterial);
	else if (DefaultMaterial)
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
	// Stealing!!
	if (_Player->PlayerStateRef->Details.Colour != OwningPlayerColor
		&& bIsNormalGrowSpot)
	{
		Stealing_Interact(_Player);
		return;
	}
	
	//SSComponent->Boing();
	
	if (!_Player->PlayerStateRef)
		return;
	
	switch(GrowSpotState)
	{
		case EGrowSpotState::Empty:
			{
				if (!_Player->HeldItem)
					break;

				switch(_Player->HeldItem->PickupActor)
				{
				case EPickupActor::SeedActor:
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

						if (ASeed* Seed = Cast<ASeed>(_Player->HeldItem))
						{
							PlantASeed(Seed);

							_Player->PlayerStateRef->AddCoins(Seed->SeedData->BabyStarValue);
						}

						//_Player->SocketWeapon(FName("Base-HumanWeapon"));

						_Player->DropItem();
						_Player->EnableStencil(false);
						break;
					}
				case EPickupActor::FertilizerActor:
					{
						if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
						{
							if (auto FertData = Fertiliser->SeedData->FertiliserData)
							{
								if (FertData->bConcrete)
								{
									FertiliseInteractDelayTimer = FertiliseInteractDelay;
									bIsFertilised = false;
									if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
										ConcretedHealth = ConcreteData->MaxStrength;
									Multi_MakePlantConcrete();
								}
								else
								{
									FertiliseInteractDelayTimer = FertiliseInteractDelay;
									ConcretedHealth = 0;
									bIsFertilised = true;
								}
							}
							_Player->PlayerStateRef->AddCoins(5);
							UpdateMaterial();
							Fertiliser->Destroy();
							_Player->DropItem();
							_Player->EnableStencil(false);
						}
						
						return;
					}
				default:
					break;
				}
				break;
			}
		case EGrowSpotState::Growing:
			{
				if (!_Player->HeldItem)
					break;
				
				switch(_Player->HeldItem->PickupActor)
				{
				case EPickupActor::FertilizerActor:
					{
						if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
						{
							if (auto FertData = Fertiliser->SeedData->FertiliserData)
							{
								if (FertData->bConcrete)
								{
									FertiliseInteractDelayTimer = FertiliseInteractDelay;
									GrowingItemRef->ItemComponent->bGold = false;
									bIsFertilised = false;
									if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
										ConcretedHealth = ConcreteData->MaxStrength;
									Multi_MakePlantConcrete();
								}
								else
								{
									if (GrowingItemRef->SeedData->BabyType == EPickupDataType::BeehiveData)
									{
										break;
									}
									else
									{
										bIsFertilised = true;
										FertiliseInteractDelayTimer = FertiliseInteractDelay;
										MakePlantGold();
									}
								}
							}
							_Player->PlayerStateRef->AddCoins(5);
							UpdateMaterial();
							Fertiliser->Destroy();
							_Player->DropItem();
							_Player->EnableStencil(false);
						}
						return;
					}
				default:
					{
						_Player->DropItem();
						_Player->EnableStencil(false);
						break;
					}
				}
				
				break;
			}
		case EGrowSpotState::Grown:
			{
				if (_Player->HeldItem)
				{
					switch(_Player->HeldItem->PickupActor)
					{
					case EPickupActor::FertilizerActor:
						{
							if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
							{
								if (auto FertData = Fertiliser->SeedData->FertiliserData)
								{
									if (FertData->bConcrete)
									{
										FertiliseInteractDelayTimer = FertiliseInteractDelay;
										GrowingItemRef->ItemComponent->bGold = false;
										bIsFertilised = false;
										if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
											ConcretedHealth = ConcreteData->MaxStrength;
										Multi_MakePlantConcrete();
									}
									else
									{
										if (GrowingItemRef->SeedData->BabyType == EPickupDataType::BeehiveData)
										{
											break;
										}
										else
										{
											FertiliseInteractDelayTimer = FertiliseInteractDelay;
											bIsFertilised = true;
											MakePlantGold();
										}
									}
								}
								_Player->PlayerStateRef->AddCoins(5);
								UpdateMaterial();
								Fertiliser->Destroy();
								_Player->DropItem();
								_Player->EnableStencil(false);
							}
							return;
						}
					default:
						{
							_Player->DropItem();
							_Player->EnableStencil(false);
							break;
						}
					}
				}

				bIsFertilised = false;
				UpdateMaterial();

				// Pickup Item
				if (GrowingItemRef && FertiliseInteractDelayTimer <= 0)
				{
					
					// causes crash atm
					//MandrakePickupNoise(_Player);
					
					// Plant

					switch(GrowingItemRef->SeedData->BabyType)
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

void AGrowSpot::HoldInteract(APrototype2Character* _Player)
{
	if (!_Player->bIsHoldingInteract)
		return;
	
	HoldInteractTimer += GetWorld()->DeltaTimeSeconds;

	// Talk to HUD through delegates
	_Player->Client_OnClaimingPlot(HoldInteractTimer, HoldInteractTotalDuration);
	
	if (HoldInteractTimer < HoldInteractTotalDuration)
	{
		return;
	}
	
	// STEAL THE ITEM
	_Player->bIsHoldingInteract = false;

	// All the pickup functionality copied from Interact function
	
	bIsFertilised = false;
	UpdateMaterial();

	// Pickup Item
	if (!GrowingItemRef || FertiliseInteractDelayTimer > 0)
	{
		return;
	}

	// Plant
	switch(GrowingItemRef->SeedData->BabyType)
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
			// Beehive currently disconnected but i'll leave this here if we decide to steal honey
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
	
	GrowSpotState = EGrowSpotState::Empty;
	GrowingItemRef = nullptr;
	GrowingActor = nullptr;	
}

void AGrowSpot::OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID)
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerID: %s"), *FString::FromInt(_PlayerID))
	//UE_LOG(LogTemp, Warning, TEXT("GrowSpotID: %s"), *FString::FromInt(Player_ID))

	if (_Owner->PlayerStateRef->Details.Colour != OwningPlayerColor
		&& bIsNormalGrowSpot)
	{
		Stealing_OnDisplayInteractText(_InvokingWidget, _Owner, _PlayerID);
		return;
	}
	
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
			else if(auto Fertiliser = Cast<AFertiliser>(_Owner->HeldItem))
			{
				if (Fertiliser->SeedData->FertiliserData->bConcrete)
					_InvokingWidget->SetHUDInteractText("Concrete");
				else
					_InvokingWidget->SetHUDInteractText("Fertilize");
				
				_Owner->EnableStencil(true);
				return;
			}
			break;
		}
	case EGrowSpotState::Growing:
		{
			auto Fertiliser = Cast<AFertiliser>(_Owner->HeldItem);
			if (Fertiliser)
			{
				if (Fertiliser->SeedData->FertiliserData->bConcrete)
					_InvokingWidget->SetHUDInteractText("Concrete");
				else
					_InvokingWidget->SetHUDInteractText("Fertilize");

				_Owner->EnableStencil(true);
				return;
			}
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			auto Fertiliser = Cast<AFertiliser>(_Owner->HeldItem);
			if (Fertiliser)
			{
				// Fert
				if (Fertiliser->SeedData->FertiliserData->bConcrete)
					_InvokingWidget->SetHUDInteractText("Concrete");
				else
					_InvokingWidget->SetHUDInteractText("Fertilize");
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

bool AGrowSpot::Stealing_IsInteractable_Unprotected(APrototype2Character* _Player)
{
	// if spotstate is NOT grown or if player has an item, return false
	if (GrowSpotState != EGrowSpotState::Grown ||
		_Player->HeldItem)
	{
		return false;
	}
	// if its a beehive, return false
	if (Cast<ABeehive>(GrowingActor))
		return false;

	return true;
}

void AGrowSpot::Stealing_ClientInteract(APrototype2Character* _Player)
{
	return;
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(_Player->HeldItem))
			{
				if (_Player->PlayerHUDRef)
					_Player->PlayerHUDRef->ClearPickupUI();

				if (GrowingItemRef)
				{
					GrowingItemRef->Client_Pickup();
				}
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
			if (GrowingItemRef)
			{
				GrowingItemRef->Client_Pickup();
			}
			if (_Player->PlayerHUDRef)
			{
				if (bIsFertilised)
				{
					if (GrowingItemRef->SeedData)
					{
						if (GrowingItemRef->SeedData->PlantData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->PlantData->GoldPlantIcon);
						}
						else if (GrowingItemRef->SeedData->WeaponData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->WeaponData->GoldWeaponIcon);
						}
					}
				}
				else
				{
					if (GrowingItemRef->SeedData)
					{
						if (GrowingItemRef->SeedData->PlantData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->PlantData->PlantIcon);
						}
						else if (GrowingItemRef->SeedData->WeaponData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->SeedData->WeaponData->WeaponIcon);
						}
					}
				}

			}
		}
	default:
		break;
	}
}

void AGrowSpot::Stealing_Interact(APrototype2Character* _Player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), "THIEF!");
	if (_Player->HeldItem)
	{
		if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
		{
			if (auto FertData = Fertiliser->SeedData->FertiliserData)
			{
				if (FertData->bConcrete)
				{
					FertiliseInteractDelayTimer = FertiliseInteractDelay;
					bIsFertilised = false;
					if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
						ConcretedHealth = ConcreteData->MaxStrength;
					Multi_MakePlantConcrete();
				}
			}
			_Player->PlayerStateRef->AddCoins(5);
			UpdateMaterial();
			Fertiliser->Destroy();
			_Player->DropItem();
			_Player->EnableStencil(false);
			return;
		}
	}

	// Stop multiple players stealing this 
	if (CurrentPlayerStealing)
		return;
		
	CurrentPlayerStealing = _Player;
	_Player->bIsHoldingInteract = true;
	HoldInteractTimer = 0.0f;
}

void AGrowSpot::Stealing_OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner,
	int32 _PlayerID)
{
	switch (GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if(auto Fertiliser = Cast<AFertiliser>(_Owner->HeldItem))
			{
				if (Fertiliser->SeedData->FertiliserData->bConcrete)
					_InvokingWidget->SetHUDInteractText("Concrete");
				
				_Owner->EnableStencil(true);
				return;
			}
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Owner->HeldItem))
			{
				if (Fertiliser->SeedData->FertiliserData->bConcrete)
					_InvokingWidget->SetHUDInteractText("Concrete");

				_Owner->EnableStencil(true);
				return;
			}
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			auto Fertiliser = Cast<AFertiliser>(_Owner->HeldItem);
			if (Fertiliser)
			{
				// Fert
				if (Fertiliser->SeedData->FertiliserData->bConcrete)
					_InvokingWidget->SetHUDInteractText("Concrete");
				_Owner->EnableStencil(true);
				return;
			}
			else if (GrowingItemRef)
			{
				_InvokingWidget->SetHUDInteractText("Steal (Hold)");
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
