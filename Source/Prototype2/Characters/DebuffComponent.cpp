

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
		if (PunchCounter > 0)
		{
			PunchCounterDropOffTimer -= DeltaTime;
			if (PunchCounterDropOffTimer <= 0)
				UpdatePunchCounter();
		}

		if (DebuffDuration > 0)
		{
			DebuffDuration -= DeltaTime;
			if (DebuffDuration <= 0)
				RemoveDebuff();
		}
	}

	if (!Player)
		return;

	if (CurrentDebuff == EDebuff::None)
		return;

	if (CurrentDebuff == EDebuff::Stun)
	{
		// Continually interrupt player beng able to interact and attack
		Server_Stun();
	}

	if (CurrentDebuff == EDebuff::Daze)
	{
		if (Player->HasIdealRole() && Player->Controller)
		{
			Server_Daze();
		}		
	}


}

void UDebuffComponent::ApplyDebuff(EDebuff _DebuffType, float _Duration)
{
	if (_DebuffType == EDebuff::None)
		return;

	// Make sure there are no current debuffs
	DebuffDuration = -1.0f;
	RemoveDebuff();
	
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
	}
}

void UDebuffComponent::Stun()
{
	// Trigger debuff VFX/animation

	// Stop player movement
	Player->bAllowMovementFromInput = false;
}

void UDebuffComponent::Server_Stun_Implementation()
{
	Player->ResetAttack();
}

void UDebuffComponent::Daze()
{
	// Trigger debuff VFX
	
	// Slightly slow player and apply ratescale
	Player->SetCharacterSpeed(DazeSpeed, DazeSpeed, DazeAnimationRateScale);
	
	// Stop player movement from input
	Player->bAllowMovementFromInput = false;
	
	DazeRandomDirection = FMath::RandRange(0,1000);
}

void UDebuffComponent::Server_Daze_Implementation()
{
	Multi_Daze();
}

void UDebuffComponent::Multi_Daze_Implementation()
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

void UDebuffComponent::Slow()
{
	// Trigger debuff VFX
	//Server_SetCharacterSpeed(SlowSpeed, SlowAnimationRateScale);
	Player->SetCharacterSpeed(SlowSpeed, SlowSpeed, SlowAnimationRateScale);
}

void UDebuffComponent::Punch()
{
	PunchCounter++;
	
	if (PunchCounter == 3)
	{
		// Reset counter and timer
		PunchCounter = 0;
		PunchCounterDropOffTimer = PunchCounterDropOff;

		// Stun 
		ApplyDebuff(EDebuff::Stun, 2.0f);
		
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
	// Allow movement again
	Player->bAllowMovementFromInput = true;
	Player->SetCharacterSpeed(Player->GetWalkSpeed(), Player->GetWalkSpeed(), Player->GetWalkRateScale());

	// Reset this bufftype
	CurrentDebuff = EDebuff::None;
}

