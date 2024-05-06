


#include "SkyAlterAttack.h"

#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Pickups/Plant.h"


// Sets default values
ASkyAlterAttack::ASkyAlterAttack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	AlterCollisionBox = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component (ROOT)"));
	SetRootComponent(AlterCollisionBox);
	AlterCollisionBox->SetSphereRadius(200.0f);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	AlterOffer_VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Alter Offer VFX Component"));
	AlterOffer_VFX->SetupAttachment(RootComponent);
	AlterOffer_VFX->SetRelativeLocation({0.0f, 0.0f, 94.0f});
	AlterOffer_VFX->SetRelativeScale3D(FVector::One() * 2.0f);
	
	AlterOffer_VFX2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Alter Offer VFX Component2"));
	AlterOffer_VFX2->SetupAttachment(RootComponent);
	AlterOffer_VFX2->SetRelativeLocation({0.0f, 0.0f, 94.0f});
	AlterOffer_VFX2->SetRelativeScale3D(FVector::One() * 2.0f);
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FoundOfferSystem(TEXT("/Game/VFX/AlphaVFX/NiagaraSystems/NS_AltarOffer"));
	if (FoundOfferSystem.Object != NULL)
	{
		AlterOffer_System = FoundOfferSystem.Object;
		AlterOffer_VFX->SetAsset(AlterOffer_System);
	}
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FoundOfferSystem2(TEXT("/Game/VFX/AlphaVFX/NiagaraSystems/NS_SmiteCountdown"));
	if (FoundOfferSystem2.Object != NULL)
	{
		AlterOffer_System2 = FoundOfferSystem2.Object;
		AlterOffer_VFX2->SetAsset(AlterOffer_System2);
	}
	
	static ConstructorHelpers::FClassFinder<AActor> PoofVFX(TEXT("/Game/Blueprints/VFX/SpawnableVFX"));
	if (PoofVFX.Class != NULL)
	{
		PoofSystem = PoofVFX.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> FoundSacrificeCue(TEXT("/Game/SFX/Altar/Sacrifice_Cue"));
	if (FoundSacrificeCue.Object != NULL)
	{
		SacrificeCue = FoundSacrificeCue.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> FoundPumpkinCue(TEXT("/Game/SFX/MostValueCrop/Pumpkin_Cue"));
	if (FoundPumpkinCue.Object != nullptr)
	{
		EnemySacrificeCue = FoundPumpkinCue.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> FoundSacrificeAttenuation(TEXT("/Game/SFX/AltarSoundAttenuation"));
	if (FoundSacrificeAttenuation.Object != NULL)
	{
		SacrificeAttenuation = FoundSacrificeAttenuation.Object;
	}

	static ConstructorHelpers::FObjectFinder<UWeaponData> FoundSmiteData(TEXT("/Game/DataAssets/WeaponData/DA_Smite"));
	if (FoundSmiteData.Object != NULL)
	{
		SmiteWeaponData = FoundSmiteData.Object;
	}
}

void ASkyAlterAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASkyAlterAttack, ServerSkyAlterData);
}

// Called when the game starts or when spawned
void ASkyAlterAttack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AlterCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASkyAlterAttack::OnPlayerTouchAltar);
	}
}

void ASkyAlterAttack::OnPlayerTouchAltar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!IsValid(OtherActor))
		return;
	
	APrototype2Character* SomePlayer = Cast<APrototype2Character>(OtherActor);
	if (!IsValid(SomePlayer))
		return;
	
	if (!IsValid(SomePlayer->HeldItem))
		return;

	auto Plant = Cast<APlant>(SomePlayer->HeldItem);
	if (!IsValid(Plant))
		return;
		
	int32 StarValue = Plant->ServerData.SeedData->BabyStarValue;
	StarValue = FMath::Clamp(StarValue, 1, 5);
	Attack(SomePlayer, StarValue);

	SomePlayer->HeldItem->Destroy();
	SomePlayer->HeldItem = nullptr;
	SomePlayer->OnRep_HeldItem();

	FServerSkyAlterData NewSkyAlterData{};
	NewSkyAlterData.TimeOfSacrifice = GetWorld()->GetTimeSeconds();
	NewSkyAlterData.LastPlayerToSacrifice = SomePlayer->GetPlayerState<APrototype2PlayerState>();
	ServerSkyAlterData = NewSkyAlterData;
	OnRep_OnSacrifice();
}

void ASkyAlterAttack::Attack(APrototype2Character* _PlayerToNotSmite, int32 _StarValueOfPlant)
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototype2Character::StaticClass(), AllPlayers);

	for(AActor* Player : AllPlayers)
	{
		// Check if player is the player sacrificing the plant
		if (Player == _PlayerToNotSmite)
		{
			continue;
		}
		
		APrototype2Character* CastedPlayer = Cast<APrototype2Character>(Player);
		if (CastedPlayer)
		{
			if (!DEBUG_AttackTeamMates)
			{
				// Check if player is team mate
				if (CastedPlayer->PlayerStateRef->Details.Colour == _PlayerToNotSmite->PlayerStateRef->Details.Colour)
					continue;
			}
			
			if (IsValid(SmiteWeaponData))
			{
				// Hit the player
				float KnockBack = KnockBackMultiplier * _StarValueOfPlant;
				CastedPlayer->InitiateSmite(KnockBack, SmiteWeaponData);
				
				UKismetSystemLibrary::PrintString(GetWorld(), "InitiatedSmite()");
			}
			else
			{
				UKismetSystemLibrary::PrintString(GetWorld(), "Invalid Smite Weapon Data");
			}
		}
		
	}
}

void ASkyAlterAttack::OnRep_OnSacrifice()
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

		/* Friendly Alter Sound VFX */
	if (LocalPlayerState->Details.Colour == ServerSkyAlterData.LastPlayerToSacrifice->Details.Colour)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SacrificeCue, GetActorLocation(),1, 1, 0, SacrificeAttenuation);
	else /* Enemy Alter Sound VFX */
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EnemySacrificeCue, GetActorLocation(),1, 1, 0, SacrificeAttenuation);
	
	// Poof VFX
	if (PoofSystem)
	{
		FVector PoofSpot = GetActorLocation();
		PoofSpot.Z = PoofSpot.Z + 130.0f;
		auto SpawnedVFX  = GetWorld()->SpawnActor<AActor>(PoofSystem, PoofSpot, FRotator{});
		SpawnedVFX->SetActorScale3D(FVector::One() * 3);
		SpawnedVFX->SetLifeSpan(5.0f);
	}

	// Alter Sacrifice VFX
	AlterOffer_VFX->Activate(true);
	AlterOffer_VFX2->Activate(true);
}