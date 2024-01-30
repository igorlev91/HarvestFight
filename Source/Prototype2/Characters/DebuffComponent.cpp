

#include "DebuffComponent.h"

#include "Prototype2Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/DataAssets/AnimationData.h"

// Sets default values for this component's properties
UDebuffComponent::UDebuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Player = Cast<APrototype2Character>(GetOuter());
}

void UDebuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDebuffComponent, DazeRandomDirection);
	DOREPLIFETIME(UDebuffComponent, PunchCounter);
	DOREPLIFETIME(UDebuffComponent, CurrentDebuff);
}


// Called when the game starts
void UDebuffComponent::BeginPlay()
{
	Super::BeginPlay();
	PunchCounterDropOffTimer = PunchCounterDropOff;
}

// Called every frame
void UDebuffComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!Player)
		return;

	DecrementTimers(DeltaTime);	
	
	if (CurrentDebuff == EDebuff::Daze)
	{
		// Make character walk in random direction
		UpdateDaze();
	}
}

void UDebuffComponent::Server_RemoveDebuff_Implementation()
{
	// Reset this bufftype
	CurrentDebuff = EDebuff::None;

	Client_ToggleDizzyVFX(false);
}

void UDebuffComponent::DecrementTimers(float _DeltaTime)
{
	if (PunchCounter > 0)
	{
		PunchCounterDropOffTimer -= _DeltaTime;
		if (PunchCounterDropOffTimer <= 0)
		{
			PunchCounterDropOffTimer = PunchCounterDropOff;
			Server_UpdatePunchCounter();
		}
	}

	if (DebuffDuration > 0)
	{
		DebuffDuration -= _DeltaTime;
		if (DebuffDuration <= 0)
		{
			Server_RemoveDebuff();
			Player->RefreshCurrentMaxSpeed();
		}
	}
}

void UDebuffComponent::Client_ToggleDizzyVFX_Implementation(bool _bTurnOn)
{
	if (_bTurnOn)
	{
		Player->ActivateParticleSystemFromEnum(EParticleSystems::Dizzy);
	}
	else
	{
		Player->DeActivateParticleSystemFromEnum(EParticleSystems::Dizzy);
	}
}

void UDebuffComponent::ApplyDebuff(EDebuff _DebuffType, float _Duration)
{
	if (_DebuffType == EDebuff::None)
		return;
	
	DebuffDuration = _Duration;
	CurrentDebuff = _DebuffType;
	
	Client_ToggleDizzyVFX_Implementation(true);
	
	switch (CurrentDebuff)
	{
	case EDebuff::Stun:
		{
			break;
		}
	case EDebuff::Daze:
		{
			Player->CurrentMaxWalkSpeed = DazeSpeed;	
			DazeRandomDirection = FMath::RandRange(0,1000);
			break;
		}
	case EDebuff::Slow:
		{
			Player->CurrentMaxWalkSpeed = SlowSpeed;
			break;
		}
	case EDebuff::Punch:
		{
			Punch();
			break;
		}
	case EDebuff::None:
		{
			break;
		}
	}
}

void UDebuffComponent::UpdateDaze()
{
	// Update direction of dizzy vector
	// Apply the dizzy vector to player movement

	if (!Player->Controller)
		return;
	
	// find out which way is forward
	const FRotator Rotation = Player->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	Player->AddMovementInput(ForwardDirection, FMath::Sin(DebuffDuration + DazeRandomDirection * 7));
	Player->AddMovementInput(RightDirection, FMath::Sin(DebuffDuration + DazeRandomDirection));
}

void UDebuffComponent::Punch()
{
	PunchCounter++;
	UKismetSystemLibrary::PrintString(GetWorld(), "Punch Counter: " + FString::FromInt(PunchCounter));

	if (PunchCounter == 3)
	{
		// Reset counter and timer
		PunchCounter = 0;
		PunchCounterDropOffTimer = PunchCounterDropOff;

		// Stun for 2s
		CurrentDebuff = EDebuff::Stun;		
		DebuffDuration = 2.0f;
		
		UKismetSystemLibrary::PrintString(GetWorld(), "Player Stunned!");
	}
}

void UDebuffComponent::Server_UpdatePunchCounter_Implementation()
{
	PunchCounter--;
	if (PunchCounter < 0)
	{
		PunchCounter = 0;
	}
}

