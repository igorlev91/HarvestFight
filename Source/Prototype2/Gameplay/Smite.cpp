


#include "Prototype2/Gameplay/Smite.h"

#include "Math/UnitConversion.h"
#include "Net/UnrealNetwork.h"

USmite::USmite()
{
	Timer = TimerStartTime;
}

USmite::~USmite()
{
}

void USmite::Tick(float DeltaTime)
{
	if (bPauseTimer)
		return;
	if (Timer > 0)
	{
		Timer -= DeltaTime;
		
		if (Timer < 0)
		{
			Timer = 0;
			Strike();
		}
	}	
}

void USmite::IncreaseTime(float _Amount)
{
	Timer += _Amount;
	if (Timer > TimerStartTime)
		Timer = TimerStartTime;
}

void USmite::SetSmiteTime(float Amount)
{
	Timer = Amount;
}

void USmite::Strike()
{
	SmiteCloud->SmtieStrike();
	Player->GetSmited(StunTime, Player->GetActorLocation(), SmiteData);
	IncreaseTime(StrikeTimeIncrease + StunTime);
}

void USmite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USmite, bPauseTimer);
}

void USmite::SetPlayer(APrototype2Character* _Player)
{
	Player = _Player;
}
