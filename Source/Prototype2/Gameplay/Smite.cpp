


#include "Prototype2/Gameplay/Smite.h"

Smite::Smite()
{
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
			Timer = 0;
	}	
}

void Smite::IncreaseTime(float _Amount)
{
	Timer += _Amount;
}

void Smite::Strike()
{
	// strike player here
}

void Smite::SetPlayer(APrototype2Character* _Player)
{
	Player = _Player;
}
