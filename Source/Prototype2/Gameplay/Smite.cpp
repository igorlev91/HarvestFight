


#include "Prototype2/Gameplay/Smite.h"

Smite::Smite()
{
	Timer = TimerStartTime;
}

Smite::~Smite()
{
}

void Smite::Tick(float DeltaTime)
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

void Smite::IncreaseTime(float _Amount)
{
	Timer += _Amount;
}

void Smite::Strike()
{
	Player->GetHit(1.0f, Player->GetActorLocation(), SmiteData);
	IncreaseTime(10);
}

void Smite::SetPlayer(APrototype2Character* _Player)
{
	Player = _Player;
}
