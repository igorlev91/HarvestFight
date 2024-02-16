

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
	SetIsReplicatedByDefault(true);
}

void UDebuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDebuffComponent, DebuffInfo);
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
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "No Player");
		return;
	}

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
	DebuffInfo.Debuff = EDebuff::None;
	DebuffInfo.Duration = 0.0f;
}

void UDebuffComponent::DecrementTimers(float _DeltaTime)
{
	if (PunchCounter > 0)
	{
		PunchCounterDropOffTimer -= _DeltaTime;
		if (PunchCounterDropOffTimer <= 0)
		{
			PunchCounterDropOffTimer = PunchCounterDropOff;
			UpdatePunchCounter();
		}
	}

	if (CurrentDebuff == EDebuff::None)
		return;
	
	if (DebuffDuration >= 0)
	{
		DebuffDuration -= _DeltaTime;
		if (DebuffDuration <= 0)
		{
			if (Player->HasAuthority())
			{
				// Reset this bufftype
				DebuffInfo.Debuff = EDebuff::None;
				DebuffInfo.Duration = 0.0f;
				OnRep_ApplyDebuff();
			}
			else
			{
				Server_RemoveDebuff();
			}
			
			Player->RefreshCurrentMaxSpeed();
		}
	}
}

void UDebuffComponent::ToggleDizzyVFX(bool _bTurnOn)
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

void UDebuffComponent::OnRep_ApplyDebuff()
{	
	CurrentDebuff = DebuffInfo.Debuff;
	//UKismetSystemLibrary::PrintString(GetWorld(), "CDebuffInfo.Debuff enum int: " + FString::FromInt( (int32)DebuffInfo.Debuff));
	DebuffDuration = DebuffInfo.Duration;	
	//UKismetSystemLibrary::PrintString(GetWorld(), "DebuffInfo.Duration: " + FString::SanitizeFloat(DebuffInfo.Duration));
	switch (CurrentDebuff)
	{
	case EDebuff::Stun:
		{
			ToggleDizzyVFX(true);
			break;
		}
	case EDebuff::Daze:
		{
			ToggleDizzyVFX(true);
			Player->SetMaxWalkSpeed(DazeSpeed);	
			DazeRandomDirection = FMath::RandRange(0,1000);
			break;
		}
	case EDebuff::Slow:
		{	
			ToggleDizzyVFX(true);
			Player->SetMaxWalkSpeed(SlowSpeed);
			break;
		}
	case EDebuff::Punch:
		{
			CurrentDebuff = EDebuff::None;
			DebuffDuration = 0.0f;
			Punch();
			break;
		}
	case EDebuff::None:
		{
			ToggleDizzyVFX(false);
			break;
		}
	}
}

void UDebuffComponent::Server_SetDebuff_Implementation(EDebuff _InDebuff, float _InDuration)
{
	DebuffInfo.Debuff = _InDebuff;
	DebuffInfo.Duration = _InDuration;
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
		if (Player->HasAuthority())
		{
			DebuffInfo.Debuff = EDebuff::Stun;
			DebuffInfo.Duration = 2.0f;
			OnRep_ApplyDebuff();
		}
		else
		{
			Server_SetDebuff(EDebuff::Stun, 2.0f);
		}
		UKismetSystemLibrary::PrintString(GetWorld(), "Player Stunned!");
	}
}

void UDebuffComponent::UpdatePunchCounter()
{
	PunchCounter--;
	if (PunchCounter < 0)
	{
		PunchCounter = 0;
	}
}

