

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
#include "Prototype2/DataAssets/WeaponData.h"
#include "Prototype2/DataAssets/PoisonFertiliserData.h"
#include "Prototype2/Pickups/Weapon.h"
#include "Prototype2/Pickups/Fertiliser.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prototype2/GameUserSettings/HarvestHavocGameUserSettings.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/Widgets/Widget_3DGrowUI.h"

AGrowSpot::AGrowSpot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InterfaceType = EInterfaceType::GrowSpot;
		
	InitComponents();
	InitAssignableVariables();
}

void AGrowSpot::InitComponents()
{
	/* CUSTOM COLLISION */
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Custom HitBox"));
	SetRootComponent(HitBox);
	HitBox->SetMobility(EComponentMobility::Movable);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetBoxExtent({80, 80, 100});
	
	/* BASE MESH / ROOT COMPONENT */
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plot Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetRelativeScale3D({0.5f, 0.5f, 1.0f});
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetRelativeLocation({0.0f, 0.0f, -95.0f});
	Mesh->CustomDepthStencilValue = 1;

	/* VFX */
	PlantReadyComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Plant Ready Sparkle"));
	PlantReadyComponent->SetupAttachment(RootComponent);
	PlantReadyComponent->bAutoActivate = false;
	PlantReadyComponent->SetRelativeLocation({0.0f, 0.0f, -20.0f});

	ConcreteBreakComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Concrete Break Particles"));
	ConcreteBreakComponent->SetupAttachment(RootComponent);
	ConcreteBreakComponent->bAutoActivate = false;
	ConcreteBreakComponent->SetRelativeLocation({0.0f, 0.0f, -20.0f});

	/* UI */
	GrowWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grow Widget Component"));
	GrowWidgetComponent->SetupAttachment(RootComponent);
	GrowWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	GrowWidgetComponent->SetRelativeLocation({0.0f, 0.0f, 100.0f});
}

void AGrowSpot::InitAssignableVariables()
{
	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		PoofSystem = PoofVFX.Class;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PlantReadyVFX(TEXT("/Game/VFX/Effects/P_VegeReady"));
	if (PlantReadyVFX.Object != NULL)
	{
		PlantReadySystem = PlantReadyVFX.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ConcreteBreakVFX(TEXT("/Game/VFX/AlphaVFX/NiagaraSystems/NS_Concrete"));
	if (ConcreteBreakVFX.Object != NULL)
	{
		ConcreteBreakComponent->SetAsset(ConcreteBreakVFX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GoldenMaterial(TEXT("/Game/Materials/PickUpMaterials/MI_Golden"));
	if (GoldenMaterial.Object != NULL)
	{
		GoldMaterial = GoldenMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> ConcreteMaterial_TEMP(TEXT("/Game/Materials/Environment_MI/MI_Grey"));
	if (ConcreteMaterial_TEMP.Object != NULL)
	{
		TEMP_ConcreteMat = ConcreteMaterial_TEMP.Object;
	}

	//static ConstructorHelpers::FObjectFinder<USoundCue> MandrakeSoundQue(TEXT("/Game/SFX/CUE_MandrakeScream"));
	//if (MandrakeSoundQue.Object != NULL)
	//{
	//	MandrakeScreamQueue = MandrakeSoundQue.Object;
	//}

	static ConstructorHelpers::FObjectFinder<USoundCue> FoundPlantCue(TEXT("/Game/SFX/CUE_PlantSeed"));
	if (FoundPlantCue.Object != NULL)
	{
		PlantCue = FoundPlantCue.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> FoundPlantAttenuation(TEXT("/Game/SFX/PlayerSoundAttenuation"));
	if (FoundPlantAttenuation.Object != NULL)
	{
		PlantAttenuation = FoundPlantAttenuation.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> MandrakeAtten(TEXT("/Game/SFX/MandrakeAttenuationSettings"));
	if (MandrakeAtten.Object != NULL)
	{
		HighValueAttenuationSettings = MandrakeAtten.Object;
	}
	
	static ConstructorHelpers::FClassFinder<AActor> PolyWeaponBP(TEXT("/Game/Blueprints/Pickups/BP_Weapon"));
	if (PolyWeaponBP.Class != NULL)
	{
		WeaponPrefab = PolyWeaponBP.Class;
	}
	static ConstructorHelpers::FClassFinder<AActor> PolyPlantBP(TEXT("/Game/Blueprints/Pickups/BP_Plant"));
	if (PolyPlantBP.Class != NULL)
	{
		PlantPrefab = PolyPlantBP.Class;
	}
	static ConstructorHelpers::FClassFinder<AActor> PolyBeehiveBP(TEXT("/Game/Blueprints/Pickups/BP_Beehive"));
	if (PolyBeehiveBP.Class != NULL)
	{
		BeehivePrefab = PolyBeehiveBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UWidget_3DGrowUI> FoundGrowUI(TEXT("/Game/Blueprints/Widgets/3D/WBP_3DPlantUI"));
	if (FoundGrowUI.Class != NULL)
	{
		GrowWidgetComponent->SetWidgetClass(FoundGrowUI.Class);
	}
}

void AGrowSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGrowSpot, GrowTimer);
	DOREPLIFETIME(AGrowSpot, GrowTime);
	DOREPLIFETIME(AGrowSpot, Beehive);
	DOREPLIFETIME(AGrowSpot, GrowSpotState);
	DOREPLIFETIME(AGrowSpot, ItemRef);
	DOREPLIFETIME(AGrowSpot, OwningPlayerColor);
	DOREPLIFETIME(AGrowSpot, FertilisationState);
	DOREPLIFETIME(AGrowSpot, ServerGrowspotDetails);
}

// Called when the game starts or when spawned
void AGrowSpot::BeginPlay()
{
	Super::BeginPlay();

	if (auto Widget = GrowWidgetComponent->GetWidget())
	{
		GrowWidget = Cast<UWidget_3DGrowUI>(Widget);
	}

	/* INIT STEAL DURATION */
	HoldDuration = 8.0f;

	/* INIT FROM DATA ASSET */
	if (GrowSpotData)
	{
		Mesh->SetStaticMesh(GrowSpotData->Mesh);
		Mesh->SetWorldScale3D(GrowSpotData->DesiredScale);
	}

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* DYNAMIC MATERIAL */
	PlotMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), nullptr);
	Mesh->SetMaterial(0, PlotMaterial);

	GrowWidgetComponent->SetVisibility(false);

	if (auto World = GetWorld())
	{
		if (auto LocalController = World->GetFirstPlayerController())
		{
			if (auto LocalCharacter = LocalController->GetCharacter())
			{
				LastLocalPlayer = Cast<APrototype2Character>(LocalCharacter);
			}
		}
	}
}

// Called every frame
void AGrowSpot::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	
	ScalePlantOnTick();
	UpdateGrowUIVisibility();

	/* SERVER ONLY */
	if (!HasAuthority())
		return;

	if (ConcreteDamageTimer > 0)
		ConcreteDamageTimer -= _DeltaTime;

	GrowPlantOnTick(_DeltaTime);
}

EInteractMode AGrowSpot::IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode)
{
	if (IsValid(_Player) == false)
		return INVALID;
	
	if /* STEALING */ (_Player->Details.Colour != OwningPlayerColor)
	{
		return IsInteractable_Stealing(_Player);
	}
	else  /* NORMAL */ 
	{
		if (CheckForSelfConcreting(_Player))
			return INVALID;
		
		return IsInteractable_Unprotected(_Player, true);
	}
}

EInteractMode AGrowSpot::IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete)
{
	if (_LookOutForConcrete && FertilisationState.ConcretedHealth > 0)
		return INVALID;
	
	if (IsValid(_Player) == false)
		return INVALID;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(_Player->GetPawn());
	if (!CastedCharacter)
		return INVALID;

	/* HOLDING FERTILIZER */
	if (CastedCharacter->IsHolding(AFertiliser::StaticClass()))
	{
		if (const auto HeldItemData = CastedCharacter->GetHeldItemData())
		{
			if (FertilisationState.bFertilised == false &&
				HeldItemData->FertiliserData->Type == EFertiliserType::GOLD)
					return INSTANT;
			
			if (HeldItemData->FertiliserData->Type == EFertiliserType::CONCRETE)
				return INSTANT;
		}

		return INVALID;
	}
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (CastedCharacter->IsHolding(ASeed::StaticClass()))
				return INSTANT;
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			if (CastedCharacter->IsHolding(ASeed::StaticClass()))
				return INVALID;
			
			if (ABeehive* SomeBeehive = Cast<ABeehive>(ItemRef))
				return SomeBeehive->IsInteractable(_Player);

			if (IsValid(ItemRef))
				return INSTANT;
		}
	default:
		break;
	}

	return INVALID;
}

EInteractMode AGrowSpot::IsInteractable_Stealing(APrototype2PlayerState* _Player)
{
	
	if (FertilisationState.ConcretedHealth > 0)
		return INVALID;
	
	if (IsValid(_Player) == false)
		return INVALID;

	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(_Player->GetPawn());
	if (!CastedCharacter)
		return INVALID;

	/* CONCRETE */
	if (CastedCharacter->IsHolding(AFertiliser::StaticClass()) &&
		CastedCharacter->GetHeldItemData() &&
		CastedCharacter->GetHeldItemData()->FertiliserData)
	{
		if (CastedCharacter->GetHeldItemData()->FertiliserData->Type == EFertiliserType::CONCRETE)
				return INSTANT;

		return INVALID;
	}
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Grown:
		{
			if (ABeehive* SomeBeehive = Cast<ABeehive>(ItemRef))
				return SomeBeehive->IsInteractable(_Player, HOLD);

			return HOLD;
		}
	default:
		break;
	}
	
	return INVALID;
}

void AGrowSpot::ClientInteract(APrototype2Character* _Player)
{
	if (IsValid(_Player) == false)
		return;

	_Player->EnableStencil(false);
	_Player->ClearPickupUI();

	if (IsValid(ItemRef) == false)
		return;
	
	auto SeedData = ItemRef->GetSeedData();
	if (IsValid(SeedData) == false)
		return;
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Grown:
		{
			switch(SeedData->BabyType)
			{
			case EPickupDataType::BeehiveData:
				{
					ABeehive* SomeBeehive = Cast<ABeehive>(ItemRef);
					SomeBeehive->ClientInteract(_Player);
					break;
				}
			default:
				break;
			}
			
			ItemRef->Client_Pickup(_Player);
		}
	default:
		break;
	}
}

void AGrowSpot::Interact(APrototype2Character* _Player)
{
	if (IsValid(_Player) == false)
		return;

	/* FERTILIZER */
	if (_Player->IsHolding(AFertiliser::StaticClass()))
	{
		if (const auto HeldItemData = _Player->GetHeldItemData())
		{
			FFertilisationState NewFertilizationState = FertilisationState;
			switch (HeldItemData->FertiliserData->Type)
			{
			case EFertiliserType::GOLD:
				{
					if (NewFertilizationState.ConcretedHealth > 0)
						return;
					NewFertilizationState.ConcretedHealth = 0;
					NewFertilizationState.bFertilised = true;

					/* UPDATE ITEM */
					if (IsValid(ItemRef))
					{
						ItemRef->ItemComponent->bGold = true;
						ItemRef->ItemComponent->OnRep_bGold();
					}
					break;
				}
			case EFertiliserType::CONCRETE:
				{
					if(const auto ConcreteData = Cast<UConcreteBagData>(HeldItemData->FertiliserData))
						NewFertilizationState.ConcretedHealth = ConcreteData->MaxStrength;
					NewFertilizationState.bFertilised = false;

					/* UPDATE ITEM */
					if (IsValid(ItemRef))
					{
						ItemRef->ItemComponent->bGold = false;
						ItemRef->ItemComponent->OnRep_bGold();
					}
					break;
				}
			default:
				break;
			}
			FertilisationState = NewFertilizationState;
			OnRep_FertilisationState();
		}

		FServerGrowspotDetails NewServerGrowspotDetails{};
		NewServerGrowspotDetails.LastTimeOfInteract = GetWorld()->GetTimeSeconds();
		NewServerGrowspotDetails.CropStarValue = 5;
		NewServerGrowspotDetails.LastPlayerToInteract = _Player->GetPlayerState<APrototype2PlayerState>();
		ServerGrowspotDetails = NewServerGrowspotDetails;
		OnRep_ServerGrowspotDetails();
		
		_Player->AddCoins(5);
		_Player->EnableStencil(false);
		
		_Player->HeldItem->Destroy();
		_Player->HeldItem = nullptr;
		_Player->OnRep_HeldItem();

		return;
	}
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if (IsValid(_Player->HeldItem) == false)
				break;

			if (_Player->IsHolding(ASeed::StaticClass()))
			{
				if (const auto HeldItemData = _Player->GetHeldItemData())
				{
					_Player->AddCoins(HeldItemData->BabyStarValue);
					
					FServerGrowspotDetails NewServerGrowspotDetails{};
					NewServerGrowspotDetails.LastTimeOfInteract = GetWorld()->GetTimeSeconds();
					NewServerGrowspotDetails.LastPlayerToInteract = _Player->GetPlayerState<APrototype2PlayerState>();
					NewServerGrowspotDetails.CropStarValue = HeldItemData->BabyStarValue;
					ServerGrowspotDetails = NewServerGrowspotDetails;
					OnRep_ServerGrowspotDetails();
				}
					

				APickUpItem* HeldSeed = _Player->HeldItem;
				_Player->HeldItem = nullptr;
				_Player->OnRep_HeldItem();
				PlantASeed(HeldSeed);
			}
			
			break;
		}
	case EGrowSpotState::Grown:
		{
			_Player->DropItem();

			if (IsValid(ItemRef) == false)
				break;
			
			switch(ItemRef->GetSeedData()->BabyType)
			{
			case EPickupDataType::WeaponData:
				{
					_Player->PickupItem(ItemRef, EPickupActor::WeaponActor);
					break;
				}
			case EPickupDataType::BeehiveData:
				{
					ABeehive* SomeBeehive = Cast<ABeehive>(ItemRef);
					SomeBeehive->Interact(_Player);
					return;
					break;
				}
			default:
				{
					_Player->PickupItem(ItemRef, EPickupActor::PlantActor);	
					break;
				}
			}
			
			GrowSpotState = EGrowSpotState::Empty;
			OnRep_GrowSpotState();
			ItemRef = nullptr;
			OnRep_ItemRef();

			FertilisationState.bFertilised = false;
			OnRep_FertilisationState();
		}
	default:
		break;
	}

	_Player->EnableStencil(false);
}

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
				_InvokingWidget->SetHUDInteractText("Steal");
				_Owner->EnableStencil(true);
			}
			else if (_Owner->HeldItem->ServerData.PickupActor == EPickupActor::FertilizerActor &&
				_Owner->HeldItem->ServerData.SeedData->FertiliserData->Type == EFertiliserType::CONCRETE)
			{
				_InvokingWidget->SetHUDInteractText("Concrete");
				_Owner->EnableStencil(true);
			}
		}
		else
		{
			_InvokingWidget->SetHUDInteractText("Steal");
			_Owner->EnableStencil(true);
		}

		return;
	}

	OnDisplayInteractText_Unprotected(_InvokingWidget, _Owner);
}

void AGrowSpot::OnDisplayInteractText_Unprotected(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner)
{
	_Owner->EnableStencil(true);

	/* FERTILIZER */
	if(_Owner->IsHolding(AFertiliser::StaticClass()))
	{
		if(const auto Fertilizer = Cast<AFertiliser>(_Owner->HeldItem))
		{
			switch (Fertilizer->ServerData.SeedData->FertiliserData->Type)
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
			default:
				break;
			}
		}

		return;
	}
	
	switch (GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			if(_Owner->IsHolding(ASeed::StaticClass()))
			{
				_InvokingWidget->SetHUDInteractText("Grow");
				break;
			}

			break;
		}
	case EGrowSpotState::Grown:
		{
			_InvokingWidget->SetHUDInteractText("Harvest");
			
			break;
		}
	default:
		break;
	}
}

void AGrowSpot::PlantASeed(ASeed* _SeedToPlant)
{
	const auto SeedData = _SeedToPlant->GetSeedData();
	if (IsValid(SeedData) == false)
		return;
	
	if (SeedData->BabyGrowTime > 0)
	{
		GrowTime = SeedData->BabyGrowTime;
		GrowTimer = SeedData->BabyGrowTime;
	}
	else
	{
		GrowTimer = 1.0f;
		GrowTime = 1.0f;
	}

	if /* RANDOM GOLD */ (FertilisationState.bFertilised == false &&
						FertilisationState.ConcretedHealth <= 0 &&
						SeedData->BabyType != EPickupDataType::BeehiveData)
	{
		if (SeedData->ChanceOfGold > 0)
		{
			if (rand() % SeedData->ChanceOfGold == 0)
			{
				FFertilisationState NewFertilisationState = FertilisationState;
				NewFertilisationState.bFertilised = true;
				FertilisationState = NewFertilisationState;
				OnRep_FertilisationState();
			}
		}
	}

	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(Mesh->GetComponentLocation()  + (FVector::UpVector * 5.0f));

	if (SeedData->BabyType == EPickupDataType::WeaponData)
	{
		//SpawnTransform.SetRotation(FQuat::MakeFromEuler({-90.0f, 0.0f, 0.0f}));
		AGrowableWeapon* NewItem = GetWorld()->SpawnActorDeferred<AGrowableWeapon>(WeaponPrefab,SpawnTransform, this);
		NewItem->SetSeedData(SeedData,EPickupActor::WeaponActor, FertilisationState.bFertilised);
		NewItem->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		UGameplayStatics::FinishSpawningActor(NewItem, SpawnTransform);
		
		ItemRef = NewItem;
		OnRep_ItemRef();
	}
	else if (SeedData->BabyType == EPickupDataType::BeehiveData)
	{
		ABeehive* NewItem = GetWorld()->SpawnActorDeferred<ABeehive>(BeehivePrefab,SpawnTransform, this);
		NewItem->ParentGrowSpot = this;
		NewItem->SetSeedData(SeedData,EPickupActor::BeehiveActor);
		NewItem->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		UGameplayStatics::FinishSpawningActor(NewItem, SpawnTransform);
		NewItem->SetBeehiveLocation(GetActorLocation());
		
		ItemRef = NewItem;
		OnRep_ItemRef();
	}
	else
	{
		APickUpItem* NewItem = GetWorld()->SpawnActorDeferred<APickUpItem>(PlantPrefab,SpawnTransform, this);
		NewItem->SetSeedData(SeedData,EPickupActor::PlantActor, FertilisationState.bFertilised);
		NewItem->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		UGameplayStatics::FinishSpawningActor(NewItem, SpawnTransform);

		ItemRef = NewItem;
		OnRep_ItemRef();
	}

	if (FertilisationState.bFertilised)
	{
		ItemRef->ItemComponent->bGold = true;
		ItemRef->ItemComponent->OnRep_bGold();
	}
	
	_SeedToPlant->Destroy();
	GrowSpotState = EGrowSpotState::Growing;
	OnRep_GrowSpotState();
}

void AGrowSpot::PlantASeed(APickUpItem* _SeedToPlant)
{
	if (auto Seed = Cast<ASeed>(_SeedToPlant))
	{
		PlantASeed(Seed);
	}
}

void AGrowSpot::GrowPlantOnTick(float _DeltaTime)
{
	if (GrowTimer > 0 && FertilisationState.ConcretedHealth <= 0)
	{
		GrowTimer -= _DeltaTime;

		if (GrowTimer <= 0)
		{
			GrowTimer = 0;
			CompleteGrowth();
		}
	}
}

void AGrowSpot::CompleteGrowth()
{
	GrowSpotState = EGrowSpotState::Grown;
	OnRep_GrowSpotState();
}

void AGrowSpot::DestroyPlant()
{
	if (IsValid(ItemRef) == false)
		return;
	
	GrowSpotState = EGrowSpotState::Empty;
	OnRep_GrowSpotState();
	ItemRef->Destroy();
	ItemRef = nullptr;
	OnRep_ItemRef();
}

bool AGrowSpot::DegradeConcrete()
{
	bool DamagedConcrete = false;
	
	if (HasAuthority() && ConcreteDamageTimer <= 0)
	{
		if (FertilisationState.ConcretedHealth > 0)
		{
			Multi_DamageConcrete();
			
			FFertilisationState NewFertilisationState = FertilisationState;
			NewFertilisationState.ConcretedHealth--;
			
			if (NewFertilisationState.ConcretedHealth <= 0)
			{
				NewFertilisationState.ConcretedHealth = 0;
			}
			ConcreteDamageTimer = 0.1f;
			DamagedConcrete = true;
			FertilisationState = NewFertilisationState;
			OnRep_FertilisationState();
		} 
	}
	
	return DamagedConcrete;
}

void AGrowSpot::Multi_DamageConcrete_Implementation()
{
	ConcreteBreakComponent->Activate(true);
}

void AGrowSpot::SpawnAPoof()
{
	if (PoofSystem)
	{
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, GetActorLocation(), FRotator{});
		SpawnedVFX->SetLifeSpan(5.0f);
	}
}

void AGrowSpot::OnRep_ItemRef()
{
	if (IsValid(LastItemRef))
	{
		if (auto SeedData = LastItemRef->GetSeedData())
		{
			if (IsValid(SeedData->HighValueCropSound))
			{
				if (HighValueAttenuationSettings)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), SeedData->HighValueCropSound, GetActorLocation(), 1, 1, 0, HighValueAttenuationSettings);
				}
				else
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), SeedData->HighValueCropSound, GetActorLocation());
				}
			}
		}
	}
	
	SetPlantReadySparkle(false);

	OnRep_FertilisationState();

	LastItemRef = ItemRef;
}

void AGrowSpot::HighValuePickupNoise()
{
	if (IsValid(ItemRef) == false ||
		IsValid(ItemRef->GetSeedData()) == false ||
		IsValid(ItemRef->GetSeedData()->HighValueCropSound) == false)
		return;
	
	if (HasAuthority())
		Multi_HighValuePickupNoise();
}

void AGrowSpot::Multi_HighValuePickupNoise_Implementation()
{
	if (HighValueAttenuationSettings)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ItemRef->GetSeedData()->HighValueCropSound, GetActorLocation(), 1, 1, 0, HighValueAttenuationSettings);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ItemRef->GetSeedData()->HighValueCropSound, GetActorLocation());
	}
}

void AGrowSpot::ScalePlantOnTick() const
{
	if (IsValid(ItemRef) == false)
		return;

	const auto SeedData = ItemRef->GetSeedData();
	if (SeedData == nullptr)
		return;

	if (SeedData->BabyType == EPickupDataType::BeehiveData)
		return;
	
	const FVector Scale = FMath::Lerp<FVector>(
		SeedData->BabyScale,
		FVector(0.2f, 0.2f, 0.2f) ,
		GrowTimer / GrowTime
		);

	ItemRef->ItemComponent->Mesh->SetWorldScale3D(Scale);
}

void AGrowSpot::UpdateGrowUI()
{
	if (IsValid(GrowWidget) == false)
		return;

	if (IsValid(ItemRef) == false)
	{
		GrowWidgetComponent->SetVisibility(false);
		return;
	}

	if (FertilisationState.ConcretedHealth > 0)
	{
		GrowWidgetComponent->SetVisibility(false);
		return;
	}

	GrowWidgetComponent->SetVisibility(true);

	if (ABeehive* SomeBeehive = Cast<ABeehive>(ItemRef))
	{
		if (SomeBeehive->TimeTillCollect > 0)
			GrowWidget->SetGrowTimer(FMath::Lerp(1.0f, 0.0f, SomeBeehive->TrackerTimeTillCollect / SomeBeehive->TimeTillCollect));
		
		GrowWidget->SetFlowerTypes(SomeBeehive->GetCloseFlowerDetails());
	}
	else if (AGrowableWeapon* SomeWeapon = Cast<AGrowableWeapon>(ItemRef))
	{
		if (GrowTime > 0)
			GrowWidget->SetGrowTimer(FMath::Lerp(1.0f, 0.0f, GrowTimer / GrowTime), FertilisationState.bFertilised);
		
		if (USeedData* SeedData = SomeWeapon->GetSeedData())
			GrowWidget->SetWeaponType((int32)SeedData->WeaponData->WeaponAnimationType);
	}
	else
	{
		if (GrowTime > 0)
			GrowWidget->SetGrowTimer(FMath::Lerp(1.0f, 0.0f, GrowTimer / GrowTime), FertilisationState.bFertilised);

		if (USeedData* SeedData = ItemRef->GetSeedData())
			GrowWidget->SetStarCount(SeedData->BabyStarValue, ItemRef->ItemComponent->bGold);
	}
}

void AGrowSpot::UpdateGrowUIVisibility()
{
	if (IsValid(LastLocalPlayer))
	{
		if (FVector::DistXY(LastLocalPlayer->GetActorLocation(), GetActorLocation()) <= UIVisiblityRadius)
			UpdateGrowUI();
		else
			GrowWidgetComponent->SetVisibility(false);
	}
	else
	{
		GrowWidgetComponent->SetVisibility(false);
	}

	if (UHarvestHavocGameUserSettings* UserSettings = UHarvestHavocGameUserSettings::GetHarvestHavocGameUserSettings())
	{
		if (UserSettings->GetPlantValueFloatingUI() == false)
		{
			GrowWidgetComponent->SetVisibility(false);
		}
	}
}

bool AGrowSpot::CheckForSelfConcreting(APrototype2PlayerState* _Player)
{
	APrototype2Character* CastedCharacter = Cast<APrototype2Character>(_Player->GetPawn());
	if (!CastedCharacter)
		return false;

	APrototype2Gamestate* CastedGamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(CastedGamestate) == false)
		return false;

	/* HOLDING FERTILIZER */
	if (CastedCharacter->IsHolding(AFertiliser::StaticClass()))
	{
		if (const auto HeldItemData = CastedCharacter->GetHeldItemData())
		{
			FHHExtraSettings& ExtraSettings = CastedGamestate->ExtraSettings;
			if (HeldItemData->FertiliserData->Type == EFertiliserType::CONCRETE &&
					ExtraSettings.bSelfConcreting == false)
			{
				return true;
			}
		}
	}

	return false;
}

void AGrowSpot::SetPlantReadySparkle(bool _bIsActive, bool _IsBeehive)
{
	if (PlantReadyComponent)
	{
		if (_bIsActive)
		{
			if (IsValid(ItemRef))
			{
				if (ItemRef->IsA(ABeehive::StaticClass())
					&& _IsBeehive == false)
					return;

				PlantReadyComponent->SetAsset(PlantReadySystem);
				
				if (auto SeedData = ItemRef->GetSeedData())
				{
					if (SeedData->WeaponData)
					{
						if (SeedData->WeaponData->PlantReadySparkle)
						{
							PlantReadyComponent->SetAsset(SeedData->WeaponData->PlantReadySparkle);
						}
					}
				}
				
				Cast<APickUpItem>(ItemRef)->SSComponent->Enable();
			}
			
			if (!PlantReadyComponent->IsActive())
			{
				PlantReadyComponent->Activate(true);
			}
		}
		else
		{
			PlantReadyComponent->DeactivateImmediate();
			if (IsValid(ItemRef))
			{
				Cast<APickUpItem>(ItemRef)->SSComponent->Disable();
			}
		}
	}
}

void AGrowSpot::OnRep_GrowSpotState()
{
	SpawnAPoof();
	
	switch(GrowSpotState)
	{
	case EGrowSpotState::Empty:
		{
			SetPlantReadySparkle(false);
			break;
		}
	case EGrowSpotState::Growing:
		{
			break;
		}
	case EGrowSpotState::Grown:
		{
			SetPlantReadySparkle(true);
			break;
		}
	default:
		break;
	}

	OnRep_FertilisationState();
}

void AGrowSpot::OnRep_FertilisationState()
{
	SpawnAPoof();
	
	if /* GOLD */ (FertilisationState.bFertilised && FertilisationState.ConcretedHealth <= 0)
	{
		Mesh->SetMaterial(0, GoldMaterial);
		
		if (GrowSpotState == EGrowSpotState::Grown)
		{
			SetPlantReadySparkle(true);
		}
	}
	else /* CONCRETE */ 
	{
		Mesh->SetMaterial(0, PlotMaterial);
		
		if (IsValid(PlotMaterial))
		{
			PlotMaterial->SetScalarParameterValue(FName("ConcreteHealth"), FertilisationState.ConcretedHealth);
		}
	
		if (FertilisationState.ConcretedHealth <= 0)
		{
			if (GrowSpotState == EGrowSpotState::Grown)
			{
				SetPlantReadySparkle(true);
			}

			/* RESET PLANT MATERIALS */
			if (IsValid(ItemRef))
			{
				if (auto SeedData = ItemRef->GetSeedData())
				{
					for(int i = 0; i < SeedData->BabyMaterials.Num(); i++)
						ItemRef->ItemComponent->Mesh->SetMaterial(i, SeedData->BabyMaterials[i]);
				}
			}
		}
		else if (FertilisationState.ConcretedHealth >= 3)
		{
			SetPlantReadySparkle(false);

			/* CONCRETE PLANT MATERIALS */
			if (IsValid(ItemRef))
			{
				if (auto SeedData = ItemRef->GetSeedData())
				{
					for(int i = 0; i < SeedData->BabyMaterials.Num(); i++)
						ItemRef->ItemComponent->Mesh->SetMaterial(i, TEMP_ConcreteMat);
				}
			}
		}
	}
}

void AGrowSpot::OnRep_ServerGrowspotDetails()
{
	auto LocalPlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(LocalPlayerController) == false)
		return;

	if (LocalPlayerController->IsLocalController() == false)
		return;

	if (GetWorld()->GetRealTimeSeconds() <= 5.0f)
		return;

	auto LocalPlayerState = LocalPlayerController->GetPlayerState<APrototype2PlayerState>();
	if (IsValid(LocalPlayerState) == false)
		return;

	APrototype2Gamestate* GamestateCast = GetWorld()->GetGameState<APrototype2Gamestate>();
	if (IsValid(GamestateCast) == false)
		return;

	if (PlantAttenuation)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PlantCue, GetActorLocation(), 1, 1, 0, HighValueAttenuationSettings);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PlantCue, GetActorLocation());
	}

	auto CropStarValue = ServerGrowspotDetails.CropStarValue;
	auto LastPlayerToInteract = ServerGrowspotDetails.LastPlayerToInteract;

	AActor* SellBinActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASellBin::StaticClass());
	if (IsValid(SellBinActor) == false)
		return;

	auto SellBinCast = Cast<ASellBin>(SellBinActor);
	if (GamestateCast->Server_Players.Contains(LastPlayerToInteract))
	{
		SellBinCast->OnItemSoldDelegate.Broadcast(GamestateCast->Server_Players.Find(LastPlayerToInteract), CropStarValue);
	}
}
