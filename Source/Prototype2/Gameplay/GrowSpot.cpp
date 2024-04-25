

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
#include "Prototype2/DataAssets/PoisonFertiliserData.h"
#include "Prototype2/Pickups/Weapon.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
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

	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		PoofSystem = PoofVFX.Class;
	}
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
	DOREPLIFETIME(AGrowSpot, bPoisoned);
}

EInteractMode AGrowSpot::IsInteractable(APrototype2PlayerState* _Player)
{
	if (ConcretedHealth > 0)
		return INVALID;
	
	if (!_Player)
		return INVALID;

	APlayerController* Controller = _Player->GetPlayerController();
	if (!Controller)
		return INVALID;

	ACharacter* Character = Controller->GetCharacter();
	if (!Character)
		return INVALID;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(Character);
	if (!CastedCharacter)
		return INVALID;

	//
	if (CastedCharacter->HeldItem &&
	CastedCharacter->HeldItem->ServerData.SeedData &&
	CastedCharacter->HeldItem->ServerData.SeedData->FertiliserData &&
	CastedCharacter->HeldItem->ServerData.SeedData->FertiliserData->Type != EFertiliserType::GOLD)
	{
		if (_Player->Details.Colour == OwningPlayerColor)
			return INVALID;

		return INSTANT;
	}

	if (_Player->Details.Colour != OwningPlayerColor)
	{
		return IsInteractable_Stealing(_Player);
	}
	
	return IsInteractable_Unprotected(_Player, true);
}

EInteractMode AGrowSpot::IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete)
{
	if (_LookOutForConcrete && ConcretedHealth > 0)
		return INVALID;
	
	if (!_Player)
		return INVALID;

	APlayerController* Controller = _Player->GetPlayerController();
	if (!Controller)
		return INVALID;

	ACharacter* Character = Controller->GetCharacter();
	if (!Character)
		return INVALID;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(Character);
	if (!CastedCharacter)
		return INVALID;
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return INSTANT;
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (!bIsFertilised)
					return INSTANT;
				else if (SomeFertilizer->ServerData.SeedData->FertiliserData->Type != EFertiliserType::GOLD)
					return INSTANT;
				else
					return INVALID;
			}
			
			break;
		}
	case EGrowSpotState::Growing:
		{
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (!bIsFertilised)
					return INSTANT;
				else if (SomeFertilizer->ServerData.SeedData->FertiliserData->Type != EFertiliserType::GOLD)
					return INSTANT;
				else
					return INVALID;
			}
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (!bIsFertilised)
					return INSTANT;
				else if (SomeFertilizer->ServerData.SeedData->FertiliserData->Type != EFertiliserType::GOLD)
					return INSTANT;
				else
					return INVALID;
			}
			if (Cast<ASeed>(CastedCharacter->HeldItem))
				return INVALID;
			
			if (ABeehive* SomeBeehive = Cast<ABeehive>(GrowingActor))
				return SomeBeehive->IsInteractable(_Player);

			return INSTANT;
		}
	default:
		{
			if (AFertiliser* SomeFertilizer = Cast<AFertiliser>(CastedCharacter->HeldItem))
			{
				if (SomeFertilizer->ServerData.SeedData->FertiliserData->Type != EFertiliserType::GOLD)
					return INSTANT;
			}
			break;
		}
	}

	return INVALID;
}

void AGrowSpot::ClientInteract(APrototype2Character* _Player)
{
	if (!_Player->PlayerStateRef)
		return;
	
	//if (_Player->PlayerStateRef->Details.Colour != OwningPlayerColor
	//	&& bIsNormalGrowSpot)
	//{
	//	Stealing_ClientInteract(_Player);
	//	return;
	//}

	_Player->EnableStencil(false);
	
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
			if (GrowingItemRef)
			{
				GrowingItemRef->Client_Pickup();
			}
			if (_Player->PlayerHUDRef)
			{
				if (bIsFertilised)
				{
					if (GrowingItemRef->ServerData.SeedData)
					{
						if (GrowingItemRef->ServerData.SeedData->PlantData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->ServerData.SeedData->PlantData->GoldPlantIcon);
						}
						else if (GrowingItemRef->ServerData.SeedData->WeaponData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->ServerData.SeedData->WeaponData->GoldWeaponIcon);
						}
					}
				}
				else
				{
					if (GrowingItemRef->ServerData.SeedData)
					{
						if (GrowingItemRef->ServerData.SeedData->PlantData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->ServerData.SeedData->PlantData->PlantIcon);
						}
						else if (GrowingItemRef->ServerData.SeedData->WeaponData)
						{
							_Player->PlayerHUDRef->UpdatePickupUI(GrowingItemRef->ServerData.SeedData->WeaponData->WeaponIcon);
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
	if (_SeedToPlant->ServerData.SeedData->BabyGrowTime > 0)
	{
		GrowTime = _SeedToPlant->ServerData.SeedData->BabyGrowTime;
		GrowTimer = _SeedToPlant->ServerData.SeedData->BabyGrowTime;
	}
	else
	{
		GrowTimer = 1.0f;
		GrowTime = 1.0f;
	}

	if (_SeedToPlant->ServerData.SeedData->BabyType == EPickupDataType::WeaponData)
	{
		AGrowableWeapon* NewItem = GetWorld()->SpawnActor<AGrowableWeapon>(WeaponPrefab);
		NewItem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewItem->SetActorRotation(FRotator{90.0f, 0.0f, 0.0f});
		NewItem->SetActorLocation(GetActorLocation() - (FVector::UpVector));
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->ServerData.SeedData,EPickupActor::WeaponActor);
	}
	else if (_SeedToPlant->ServerData.SeedData->BabyType == EPickupDataType::BeehiveData)
	{
		ABeehive* NewItem = GetWorld()->SpawnActor<ABeehive>(BeehivePrefab);
		NewItem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewItem->SetActorLocation(GetActorLocation() + (FVector::UpVector * 5.0f));
		NewItem->SetBeehiveLocation(GetActorLocation());
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->ServerData.SeedData,EPickupActor::BeehiveActor);
		NewItem->ParentGrowSpot = this;
	}
	else
	{
		APickUpItem* NewItem = GetWorld()->SpawnActor<APickUpItem>(PlantPrefab);
		NewItem->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewItem->SetActorLocation(GetActorLocation() + (FVector::UpVector * 5.0f));
		GrowingActor = NewItem;
		GrowingItemRef = NewItem;
		GrowingItemRef->SetSeedData(_SeedToPlant->ServerData.SeedData, EPickupActor::PlantActor);
	}
	
	GrowingItemRef->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Init random gold
	if (bIsFertilised)
	{
		MakePlantGold();
	}
	else if (_SeedToPlant->ServerData.SeedData->BabyType != EPickupDataType::BeehiveData)
	{
		int32 X = rand() % 100;
		//X = 0;
		if (X < GrowingItemRef->ServerData.SeedData->ChanceOfGold)
		{
			bIsFertilised = true;
			GrowingItemRef->ItemComponent->bGold = true;
			MakePlantGold();
		}
	}
		
	GrowingItemRef->ItemComponent->Mesh->SetSimulatePhysics(false);
	GrowingItemRef->ItemComponent->Multi_DisableCollisionAndAttach();
	
	_SeedToPlant->Destroy();

	Multi_PlantASeed();
	
	GrowSpotState = EGrowSpotState::Growing;

	
}

void AGrowSpot::Multi_PlantASeed_Implementation()
{
	if (PoofSystem)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, GetActorLocation(), FRotator{});
		SpawnedVFX->SetLifeSpan(5.0f);
	}
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

bool AGrowSpot::DegradeConcrete()
{
	bool ConcreteDegraded = false;
	if (ConcretedHealth > 0 && ConcretedDamageTimer <= 0)
	{
		ConcreteDegraded = true;
	}
	if (HasAuthority())
	{
		if (ConcretedHealth > 0 && ConcretedDamageTimer <= 0)
		{
			ConcretedDamageTimer = ConcretedDamageInterval;
			ConcretedHealth--;
			OnRep_ConcreteHealth();
			if (ConcretedHealth <= 0)
			{
				ConcretedHealth = 0;

				if (IsValid(GrowingItemRef))
					GrowingItemRef->ItemComponent->bGold = false;
			}
		}
	}
	else
	{
		Server_DegradeConcrete();
	}
	return ConcreteDegraded;
}

void AGrowSpot::Server_DegradeConcrete_Implementation()
{
	if (ConcretedHealth > 0 && ConcretedDamageTimer <= 0)
	{
		ConcretedDamageTimer = ConcretedDamageInterval;
		ConcretedHealth--;
		OnRep_ConcreteHealth();
		if (ConcretedHealth <= 0)
		{
			ConcretedHealth = 0;

			//
			if (IsValid(GrowingItemRef))
				GrowingItemRef->ItemComponent->bGold = false;
		}
	}
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();

	/* INIT STEAL DURATION */
	HoldDuration = 8.0f;

	if (GrowSpotData)
	{
		ItemComponent->Mesh->SetStaticMesh(GrowSpotData->Mesh);
		ItemComponent->Mesh->SetWorldScale3D(GrowSpotData->DesiredScale);
	}

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


	{ /* DYNAMIC MATERIAL */
		
		PlotMaterial = UMaterialInstanceDynamic::Create(ItemComponent->Mesh->GetMaterial(0), nullptr);
		ItemComponent->Mesh->SetMaterial(0, PlotMaterial);
	} /* DYNAMIC MATERIAL */
	
	{ /* SERVER ONLY */
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
	} /* SERVER ONLY */
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

	//if (CurrentPlayerStealing)
	//{
	//	if (CurrentPlayerStealing->bIsHoldingInteract == false)
	//	{
	//		// Talk to HUD through delegates
	//		CurrentPlayerStealing->Client_OnStoppedClaimingPlot();
//
	//		CurrentPlayerStealing = nullptr;
	//	}
	//}
}

void AGrowSpot::Multi_MakePlantGold_Implementation()
{
	//if (!GrowingItemRef)
	//	return;
//
	//for (int i = 0; i < GrowingItemRef->ItemComponent->Mesh->GetNumMaterials(); i++)
	//{
	//	if (GrowingItemRef->ServerData.SeedData->BabyGoldMaterials.Num() > i)
	//		GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->ServerData.SeedData->BabyGoldMaterials[i]);
	//	else if (GrowingItemRef->ServerData.SeedData->BabyGoldMaterials.Num() > 0)
	//		GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, GrowingItemRef->ServerData.SeedData->BabyGoldMaterials[0]);
	//}
}

void AGrowSpot::MandrakePickupNoise(APrototype2Character* _Player)
{
	if (!_Player->HeldItem)
	{
		return;
	}

	if (!_Player->HeldItem->ServerData.SeedData->BabyMesh)
	{
		return;
	}

	if (_Player->HeldItem->ServerData.SeedData->Name.Compare("Mandrake"))
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
	if (GrowingItemRef  == nullptr)
		return;

	if (GrowingItemRef->ServerData.SeedData == nullptr)
		return;

	if (GrowingItemRef->ServerData.SeedData->BabyType == EPickupDataType::BeehiveData)
		return;
	
	const FVector Scale = FMath::Lerp<FVector>(GrowingItemRef->ServerData.SeedData->BabyScale, FVector(0.2f, 0.2f, 0.2f) , GrowTimer / GrowTime);

	GrowingItemRef->ItemComponent->Mesh->SetWorldScale3D(Scale);
}

void AGrowSpot::MakePlantGold()
{
	if (!GrowingItemRef)
		return;

	if (HasAuthority())
		GrowingItemRef->ItemComponent->bGold = true;
	
	UpdateMaterial();
}

void AGrowSpot::Server_MakePlantGold_Implementation()
{
	GrowingItemRef->ItemComponent->bGold = true;
	//Multi_MakePlantGold();
}

void AGrowSpot::Multi_MakePlantConcrete_Implementation()
{
}

void AGrowSpot::Multi_BrakePlantConcrete_Implementation()
{
	
}

void AGrowSpot::Multi_MakePlantPoison_Implementation()
{
	if (!GrowingItemRef || !PoisonMaterial)
		return;
	
	for (int i = 0; i < GrowingItemRef->ItemComponent->Mesh->GetNumMaterials(); i++)
	{
		GrowingItemRef->ItemComponent->Mesh->SetMaterial(i, PoisonMaterial);
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
	if (ConcretedHealth > 0)
		return;
	else if (bIsFertilised && GoldMaterial)
		ItemComponent->Mesh->SetMaterial(0, GoldMaterial);
	else if (bPoisoned && PoisonMaterial)
		ItemComponent->Mesh->SetMaterial(0, PoisonMaterial);
	else if (PlotMaterial)
		ItemComponent->Mesh->SetMaterial(0, PlotMaterial);
}

void AGrowSpot::OnRep_ConcreteHealth()
{
	if (IsValid(PlotMaterial))
	{
		PlotMaterial->SetScalarParameterValue(FName("ConcreteHealth"), ConcretedHealth);
	}
	
	if (ConcretedHealth <= 0)
	{
		if (GrowSpotState == EGrowSpotState::Grown)
		{
			PlantReadyComponent->Activate();

			if (IsValid(GrowingItemRef))
				GrowingItemRef->SSComponent->Enable();
		}
	}
	else if (ConcretedHealth >= 3)
	{
		PlantReadyComponent->DeactivateImmediate();

		if (IsValid(GrowingItemRef))
			GrowingItemRef->SSComponent->Disable();
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
	_Player->EnableStencil(false);
	
	if (!_Player->PlayerStateRef)
		return;
	
	switch(GrowSpotState)
	{
		case EGrowSpotState::Empty:
			{
				if (!_Player->HeldItem)
					break;

				switch(_Player->HeldItem->ServerData.PickupActor)
				{
				case EPickupActor::SeedActor:
					{
						if (!_Player->HeldItem->ServerData.SeedData)
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
							_Player->DropItem();
							PlantASeed(Seed);

							_Player->PlayerStateRef->AddCoins(Seed->ServerData.SeedData->BabyStarValue);
						}

						//_Player->SocketWeapon(FName("Base-HumanWeapon"));

						_Player->EnableStencil(false);
						break;
					}
				case EPickupActor::FertilizerActor:
					{
						if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
						{
							if (auto FertData = Fertiliser->ServerData.SeedData->FertiliserData)
							{
								FertiliseInteractDelayTimer = FertiliseInteractDelay;
								
								switch (FertData->Type)
								{
								case EFertiliserType::GOLD:
									{
										ConcretedHealth = 0;
										bIsFertilised = true;
										break;
									}
								case EFertiliserType::CONCRETE:
									{
										bIsFertilised = false;
										if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
											ConcretedHealth = ConcreteData->MaxStrength;
										OnRep_ConcreteHealth();
										break;
									}
								case EFertiliserType::POISON:
									{
										bIsFertilised = false;
										if(auto PoisonData = Cast<UPoisonFertiliserData>(FertData))
										{
											bPoisoned = true;
										}
										Multi_MakePlantPoison();
										break;
									}
								default:
									break;
								}
							}
							_Player->PlayerStateRef->AddCoins(5);
							UpdateMaterial();
							_Player->DropItem();
							Fertiliser->Destroy();
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
				
				switch(_Player->HeldItem->ServerData.PickupActor)
				{
				case EPickupActor::FertilizerActor:
					{
						if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
						{
							if (auto FertData = Fertiliser->ServerData.SeedData->FertiliserData)
							{
								FertiliseInteractDelayTimer = FertiliseInteractDelay;
								
								switch (FertData->Type)
								{
								case EFertiliserType::GOLD:
									{
										if (GrowingItemRef->ServerData.SeedData->BabyType == EPickupDataType::BeehiveData)
										{
											break;
										}
										else
										{
											bIsFertilised = true;
											MakePlantGold();
										}
										break;
									}
								case EFertiliserType::CONCRETE:
									{
										GrowingItemRef->ItemComponent->bGold = false;
										bIsFertilised = false;
										if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
											ConcretedHealth = ConcreteData->MaxStrength;
										OnRep_ConcreteHealth();
										break;
									}
								case EFertiliserType::POISON:
									{
										GrowingItemRef->ItemComponent->bGold = false;
										bIsFertilised = false;
										bPoisoned = true;
										if (APlant* SomePlant = Cast<APlant>(GrowingItemRef))
										{
											SomePlant->bPoisoned = true;
										}
										Multi_MakePlantPoison();
										break;
									}
								default:
									break;
								}
							}
							_Player->PlayerStateRef->AddCoins(5);
							UpdateMaterial();
							_Player->DropItem();
							Fertiliser->Destroy();
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
					switch(_Player->HeldItem->ServerData.PickupActor)
					{
					case EPickupActor::FertilizerActor:
						{
							if (AFertiliser* Fertiliser = Cast<AFertiliser>(_Player->HeldItem))
							{
								if (auto FertData = Fertiliser->ServerData.SeedData->FertiliserData)
								{
									FertiliseInteractDelayTimer = FertiliseInteractDelay;
								
									switch (FertData->Type)
									{
										case EFertiliserType::GOLD:
											{
												if (GrowingItemRef->ServerData.SeedData->BabyType == EPickupDataType::BeehiveData)
												{
													break;
												}
												else
												{
													bIsFertilised = true;
													MakePlantGold();
												}
												break;
											}
										case EFertiliserType::CONCRETE:
											{
												GrowingItemRef->ItemComponent->bGold = false;
												bIsFertilised = false;
												if(auto ConcreteData = Cast<UConcreteBagData>(FertData))
													ConcretedHealth = ConcreteData->MaxStrength;
												OnRep_ConcreteHealth();
												break;
											}
										case EFertiliserType::POISON:
											{
												GrowingItemRef->ItemComponent->bGold = false;
												bIsFertilised = false;
												bPoisoned = true;
												if (APlant* SomePlant = Cast<APlant>(GrowingItemRef))
												{
													SomePlant->bPoisoned = true;
												}
												//Multi_MakePlantPoison();
												break;
											}
										default:
											break;
									}
								}
								_Player->PlayerStateRef->AddCoins(5);
								UpdateMaterial();
								_Player->DropItem();
								Fertiliser->Destroy();
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

					switch(GrowingItemRef->ServerData.SeedData->BabyType)
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

//void AGrowSpot::HoldInteract(APrototype2Character* _Player)
//{
//	if (!_Player->bIsHoldingInteract)
//		return;
//	
//	HoldInteractTimer += GetWorld()->DeltaTimeSeconds;
//
//	// Talk to HUD through delegates
//	_Player->Client_OnClaimingPlot(HoldInteractTimer, HoldInteractTotalDuration);
//	
//	if (HoldInteractTimer < HoldInteractTotalDuration)
//	{
//		return;
//	}
//	
//	// STEAL THE ITEM
//	_Player->bIsHoldingInteract = false;
//
//	// All the pickup functionality copied from Interact function
//	
//	bIsFertilised = false;
//	UpdateMaterial();
//
//	// Pickup Item
//	if (!GrowingItemRef || FertiliseInteractDelayTimer > 0)
//	{
//		return;
//	}
//
//	// Plant
//	switch(GrowingItemRef->ServerData.SeedData->BabyType)
//	{
//	case EPickupDataType::PlantData:
//		{
//			APlant* SomePlant = Cast<APlant>(GrowingItemRef);
//			SomePlant->bGrown = true;
//			_Player->PickupItem(GrowingItemRef, EPickupActor::PlantActor);
//			_Player->EnableStencil(false);
//
//			break;
//		}
//	case EPickupDataType::WeaponData:
//		{
//			AGrowableWeapon* SomeWeapon = Cast<AGrowableWeapon>(GrowingItemRef);
//			SomeWeapon->bGrown = true;
//			_Player->PickupItem(GrowingItemRef, EPickupActor::WeaponActor);
//			_Player->EnableStencil(false);
//					
//			break;
//		}
//	case EPickupDataType::BeehiveData:
//		{
//			// Beehive currently disconnected but i'll leave this here if we decide to steal honey
//			ABeehive* SomeBeehive = Cast<ABeehive>(GrowingItemRef);
//			SomeBeehive->bGrown = true;
//			SomeBeehive->Interact(_Player);
//			_Player->EnableStencil(false);
//					
//			if (SomeBeehive->HarvestedHoney < SomeBeehive->MaxHarvestableHoney)
//			{
//				return;
//			}
//
//			SomeBeehive->Destroy();
//			break;
//		}
//	case EPickupDataType::FlowerData:
//		{
//			APlant* SomePlant = Cast<APlant>(GrowingItemRef);
//			SomePlant->bGrown = true;
//			_Player->PickupItem(GrowingItemRef, EPickupActor::PlantActor);
//			_Player->EnableStencil(false);
//			break;
//		}
//	default:
//		{
//			_Player->PickupItem(GrowingItemRef, EPickupActor::Default);
//			_Player->EnableStencil(false);
//			break;
//		}
//	}			
//			
//	ItemComponent->Mesh->SetRenderCustomDepth(false);
//	Multi_SetPlantReadySparkle(false);
//	
//	GrowSpotState = EGrowSpotState::Empty;
//	GrowingItemRef = nullptr;
//	GrowingActor = nullptr;	
//}

void AGrowSpot::OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int32 _PlayerID)
{
	/* STEALING TEXT */
	if (_Owner->GetPlayerState<APrototype2PlayerState>()->Details.Colour != OwningPlayerColor
		&& GrowSpotState == EGrowSpotState::Grown)
	{
		if (IsValid(_Owner->HeldItem))
		{
			if (_Owner->HeldItem->ServerData.PickupActor != EPickupActor::FertilizerActor)
			{
				_Owner->EnableStencil(true);
				_InvokingWidget->SetHUDInteractText("Steal");
				return;
			}
		}
		else
		{
			
			_Owner->EnableStencil(true);
			_InvokingWidget->SetHUDInteractText("Steal");
			return;
		}
	}
	//

	OnDisplayInteractText_Unprotected(_InvokingWidget, _Owner);
}

void AGrowSpot::OnDisplayInteractText_Unprotected(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner)
{
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
				switch (Fertiliser->ServerData.SeedData->FertiliserData->Type)
				{
				case EFertiliserType::GOLD:
					{
						_InvokingWidget->SetHUDInteractText("Fertilize");
						break;
					}
				case EFertiliserType::CONCRETE:
					{
						_InvokingWidget->SetHUDInteractText("Concrete");
						break;
					}
				case EFertiliserType::POISON:
					{
						_InvokingWidget->SetHUDInteractText("Poison");				
						break;
					}
				default:
					break;
				}

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
				switch (Fertiliser->ServerData.SeedData->FertiliserData->Type)
				{
				case EFertiliserType::GOLD:
					{
						_InvokingWidget->SetHUDInteractText("Fertilize");
						break;
					}
				case EFertiliserType::CONCRETE:
					{
						_InvokingWidget->SetHUDInteractText("Concrete");
						break;
					}
				case EFertiliserType::POISON:
					{
						_InvokingWidget->SetHUDInteractText("Poison");				
						break;
					}
				default:
					break;
				}

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
				switch (Fertiliser->ServerData.SeedData->FertiliserData->Type)
				{
				case EFertiliserType::GOLD:
					{
						_InvokingWidget->SetHUDInteractText("Fertilize");
						break;
					}
				case EFertiliserType::CONCRETE:
					{
						_InvokingWidget->SetHUDInteractText("Concrete");
						break;
					}
				case EFertiliserType::POISON:
					{
						_InvokingWidget->SetHUDInteractText("Poison");				
						break;
					}
				default:
					break;
				}
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

void AGrowSpot::OnClientWalkAway(APrototype2Character* _Player)
{
	ItemComponent->Mesh->bRenderCustomDepth = false;
}

EInteractMode AGrowSpot::IsInteractable_Stealing(APrototype2PlayerState* _Player)
{
	if (!IsValid(_Player))
		return INVALID;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(_Player->GetPawn());
	if (!IsValid(CastedCharacter))
		return INVALID;

	if (CastedCharacter->HeldItem &&
	CastedCharacter->HeldItem->ServerData.SeedData &&
	CastedCharacter->HeldItem->ServerData.SeedData->FertiliserData)
	{
		if (CastedCharacter->HeldItem->ServerData.SeedData->FertiliserData->Type != EFertiliserType::GOLD)
			return INSTANT;
		else
			return INVALID;
	}
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Grown:
		{
			//
			// TO-DO Add beehive Stealing IsInteractable()
			//
			//if (ABeehive* SomeBeehive = Cast<ABeehive>(GrowingActor))
			//	return SomeBeehive->IsInteractable(_Player);

			return HOLD;
		}
	default:
		break;
	}
	
	return INVALID;
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
			if (IsValid(GrowingItemRef))
			{
				Cast<APickUpItem>(GrowingActor)->SSComponent->Enable();
			}
		}
		else
		{
			PlantReadyComponent->Deactivate();
			if (IsValid(GrowingItemRef))
			{
				Cast<APickUpItem>(GrowingActor)->SSComponent->Disable();
			}
		}
	}
}
