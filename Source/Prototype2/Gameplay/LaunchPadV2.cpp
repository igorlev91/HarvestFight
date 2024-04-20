

#include "LaunchPadV2.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2/VFX/SquashAndStretch.h"

ALaunchPadV2::ALaunchPadV2()
{
	PrimaryActorTick.bCanEverTick = true;

	LaunchCollider = CreateDefaultSubobject<USphereComponent>("Launch Collider");
	SetRootComponent(LaunchCollider);
	LaunchCollider->SetSphereRadius(70.0f);
	LaunchCollider->OnComponentBeginOverlap.AddDynamic(this, &ALaunchPadV2::BeginOverlap);

	LaunchPadMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	LaunchPadMesh->SetupAttachment(RootComponent);
	LaunchPadMesh->SetCollisionProfileName("NoCollision");

	SquashAndStretch = CreateDefaultSubobject<USquashAndStretch>(TEXT("Squash And Stretch Component"));

	LaunchDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Launch Direction"));
	LaunchDirection->SetupAttachment(RootComponent);
	LaunchDirection->SetRelativeLocation(FVector::UpVector * 75.0f);

	AssignComponentDefaults();
}

void ALaunchPadV2::AssignComponentDefaults()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> FoundLaunchCue(TEXT("/Game/SFX/LaunchPad/launchpad_Cue"));
	if (FoundLaunchCue.Object != nullptr)
	{
		LaunchCue = FoundLaunchCue.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> FoundPlayerAttenuation(TEXT("/Game/SFX/PlayerSoundAttenuation"));
	if (FoundPlayerAttenuation.Object != nullptr)
	{
		LaunchAttenuation = FoundPlayerAttenuation.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMushroomMesh(TEXT("/Game/Environment/SkyIslandAssets/SM_Mushroom_Launch_Pad"));
	if (FoundMushroomMesh.Object != nullptr)
	{
		LaunchPadMesh->SetStaticMesh(FoundMushroomMesh.Object);
	}
}

void ALaunchPadV2::BeginPlay()
{
	Super::BeginPlay();

	SquashAndStretch->SetBoingMagnitude(0.05f);
	SquashAndStretch->SetAxis({1,1,0});
	SquashAndStretch->SetMeshToStretch(LaunchPadMesh);
	SquashAndStretch->Enable();
}

void ALaunchPadV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALaunchPadV2::BeginOverlap(UPrimitiveComponent* _OverlappedComponent, AActor* _OtherActor,
	UPrimitiveComponent* _OtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	if (IsValid(_OtherActor) == false)
		return;

	if /* SOME ITEM */ (APickUpItem* SomeItem = Cast<APickUpItem>(_OtherActor))
	{
		PlayLaunchSound();
		Launch(SomeItem);
	}
	else if /* SOME PLAYER */ (APrototype2Character* SomeCharacter = Cast<APrototype2Character>(_OtherActor))
	{
		PlayLaunchSound();
		Launch(SomeCharacter);
	}
}

void ALaunchPadV2::Launch(APickUpItem* _Item)
{
	FVector LaunchVector = FVector::UpVector * VerticalStrength;
	_Item->ItemComponent->Mesh->AddImpulse(LaunchVector, NAME_None, true);
}

void ALaunchPadV2::Launch(APrototype2Character* _Player)
{
	FVector LaunchVector = FVector::UpVector * VerticalStrength;

	if (bForceVertical == false
		&& FVector::DistXY({}, _Player->GetVelocity()) > 100.0f)
	{
		if (bLaunchWithArrowDirection)
			LaunchVector += LaunchDirection->GetForwardVector() * ForwardStrength;
		else
			LaunchVector += _Player->GetVelocity().GetSafeNormal2D() * ForwardStrength;
	}

	_Player->OnLaunchedByLaunchPad();
	
	_Player->LaunchCharacter(LaunchVector, false, true);
}

void ALaunchPadV2::PlayLaunchSound()
{
	if (LaunchCue == nullptr)
		return;
	
	if (LaunchAttenuation == nullptr)
		return;
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchCue, GetActorLocation(), 1, 1, 0, LaunchAttenuation);
}

