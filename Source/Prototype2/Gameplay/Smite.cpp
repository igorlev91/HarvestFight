


#include "Prototype2/Gameplay/Smite.h"

USmite::USmite()
{
	Timer = TimerStartTime;
}

USmite::~USmite()
{
}

void USmite::Tick(float DeltaTime)
{
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
}

void USmite::SetSmiteTime(float Amount)
{
	Timer = Amount;
}

void USmite::Strike()
{
	Player->GetHit(1.0f, Player->GetActorLocation(), SmiteData);
	IncreaseTime(StrikeTimeIncrease);
}

void USmite::SetPlayer(APrototype2Character* _Player)
{
	Player = _Player;
}
