

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
	DOREPLIFETIME(UDebuffComponent, DebuffDuration);
	DOREPLIFETIME(UDebuffComponent, DazeRandomDirection);
	DOREPLIFETIME(UDebuffComponent, PunchCounterDropOffTimer);
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

	if (Player->HasAuthority())
	{
		Server_DecrementTimers(DeltaTime);
	}

	if (!Player)
		return;

	if (CurrentDebuff == EDebuff::None)
		return;

	if (CurrentDebuff == EDebuff::Stun)
	{
		// Continually interrupt player beng able to interact and attack
		Server_UpdateStun();
	}

	if (CurrentDebuff == EDebuff::Daze)
	{
		Server_UpdateDaze();
	}
}

void UDebuffComponent::ApplyDebuff(EDebuff _DebuffType, float _Duration)
{
	Server_ApplyDebuff(_DebuffType, _Duration);
}

void UDebuffComponent::Server_ApplyDebuff_Implementation(EDebuff _DebuffType, float _Duration)
{
	if (_DebuffType == EDebuff::None)
		return;
	
	DebuffDuration = _Duration;
	CurrentDebuff = _DebuffType;
	
	switch (CurrentDebuff)
	{
	case EDebuff::Stun:
		{
			Stun();
			break;
		}
	case EDebuff::Daze:
		{
			Daze();
			break;
		}
	case EDebuff::Slow:
		{
			Slow();
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

void UDebuffComponent::Stun()
{
	// Trigger debuff VFX/animation

	// Stop player movement
	Player->bAllowMovementFromInput = false;
}

void UDebuffComponent::Server_UpdateStun_Implementation()
{
	Player->ResetAttack();
}

void UDebuffComponent::Daze()
{
	// Trigger debuff VFX
	
	// Slightly slow player and apply ratescale
	Player->CurrentMaxWalkSpeed = DazeSpeed;
	
	// Stop player movement from input
	Player->bAllowMovementFromInput = false;
	
	DazeRandomDirection = FMath::RandRange(0,1000);
}

void UDebuffComponent::Server_UpdateDaze_Implementation()
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
	
	//Multi_Daze();
}

void UDebuffComponent::Multi_Daze_Implementation()
{
}

void UDebuffComponent::Slow()
{
	// Trigger debuff VFX/
	Player->CurrentMaxWalkSpeed = SlowSpeed;
}

void UDebuffComponent::Punch()
{
	PunchCounter++;
	
	if (PunchCounter == 3)
	{
		// Reset counter and timer
		PunchCounter = 0;
		PunchCounterDropOffTimer = PunchCounterDropOff;

		// Stun for 2s
		CurrentDebuff = EDebuff::Stun;		
		Player->bAllowMovementFromInput = false;
		DebuffDuration = 2.0f;
		
		UKismetSystemLibrary::PrintString(GetWorld(), "Player Stunned!");
	}
}

void UDebuffComponent::UpdatePunchCounter()
{
	PunchCounterDropOffTimer = PunchCounterDropOff;
	PunchCounter--;
	if (PunchCounter < 0)
	{
		PunchCounter = 0;
	}
}

void UDebuffComponent::RemoveDebuff()
{
	Server_RemoveDebuff();
	
	Player->RefreshCurrentMaxSpeed();
}

void UDebuffComponent::Server_RemoveDebuff_Implementation()
{
	// Allow movement again
	Player->bAllowMovementFromInput = true;

	// Reset this bufftype
	CurrentDebuff = EDebuff::None;
}

void UDebuffComponent::Server_DecrementTimers_Implementation(float _DeltaTime)
{
	if (PunchCounter > 0)
	{
		PunchCounterDropOffTimer -= _DeltaTime;
		if (PunchCounterDropOffTimer <= 0)
			UpdatePunchCounter();
	}

	if (DebuffDuration > 0)
	{
		DebuffDuration -= _DeltaTime;
		if (DebuffDuration <= 0)
			RemoveDebuff();
	}
}

