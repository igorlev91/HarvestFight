

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Characters/Prototype2Character.h"


class PROTOTYPE2_API Smite
{
public:
	Smite();
	~Smite();

	void Tick(float DeltaTime);

	static constexpr float TimerStartTime{60};
	float Timer{0};

	APrototype2Character* Player{nullptr};

	void IncreaseTime(float _Amount);
	void Strike();

	void SetPlayer(APrototype2Character* _Player);
	
};
