

#include "LaunchPad.h"

#include "Components/BoxComponent.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/VFX/SquashAndStretch.h"


// Sets default values
ALaunchPad::ALaunchPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LaunchPadMesh = CreateDefaultSubobject<UStaticMeshComponent>("LaunchPadMesh");
	SetRootComponent(LaunchPadMesh);

	LaunchCollider = CreateDefaultSubobject<UBoxComponent>("LaunchCollider");
	LaunchCollider->SetupAttachment(RootComponent);

	SquashAndStretch = CreateDefaultSubobject<USquashAndStretch>("Squash And Stretch Component");
}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();

	SquashAndStretch->SetBoingMagnitude(0.05f);
	SquashAndStretch->SetAxis({1,1,0});
	SquashAndStretch->SetMeshToStretch(LaunchPadMesh);
	SquashAndStretch->Enable();
}

// Called every frame
void ALaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Counter -= DeltaTime;

	if (!bOnAndOff)
		return;

	if (InitialDelay > 0)
	{
		InitialDelay -= DeltaTime;
		return;
	}

	if (bTurnedOn)
	{
		Timer -= DeltaTime;
		if (Timer <= 0)
		{
			Timer = TimeOff;
			bTurnedOn = false;
			this->SetHidden(true);
		}
	}
	else
	{
		Timer -= DeltaTime;
		if (Timer <= 0)
		{
			Timer = TimeOn;
			bTurnedOn = true;
			this->SetHidden(false);
		}
	}
}

void ALaunchPad::Launch(APrototype2Character* _Player, bool _WithArrowDirection)
{
	if (Counter > 0)
		return;
	else
	{
		Counter = Delay;
	}

	if (bOnAndOff)
	{
		if (!bTurnedOn)
			return;
	}
	
	// Set which direction the player will be launched according to _WithArrowDirection
	FVector LaunchVector = GetActorForwardVector();
	if (!_WithArrowDirection)
	{
		LaunchVector = _Player->GetActorForwardVector();
	}

	float DistanceBetweenUs = (_Player->GetActorLocation() - GetActorLocation()).Length();
	
	if (!bOnlyUp)
	{
		LaunchVector *= ForwardStrength +
		// basically nullify the players velocity while keeping the walkspeed velocity
		// which is what all the jump pads would have been set up with I'm assuming
		// And just give them another little kick at the end for good measure
		500.0f /* Player max walk speed */ - _Player->GetVelocity().Length() + 100.0f;
	}
	else
	{
		LaunchVector = {0,0,0};
	}
	
	LaunchVector.Z = VerticalStrength;

	if (HasAuthority())
	{
		if (bIsVent)
		{
			_Player->Client_PlaySoundAtLocation(_Player->GetActorLocation(), _Player->AirVentPadCue);
		}
		else
		{
			_Player->Client_PlaySoundAtLocation(_Player->GetActorLocation(), _Player->LaunchPadCue);
		}
	}
	else
	{
		if (bIsVent)
		{
			_Player->PlaySoundAtLocation(_Player->GetActorLocation(), _Player->AirVentPadCue);
		}
		else
		{
			_Player->PlaySoundAtLocation(_Player->GetActorLocation(), _Player->LaunchPadCue);
		}
	}
	_Player->LaunchCharacter(LaunchVector, false, false);
}

