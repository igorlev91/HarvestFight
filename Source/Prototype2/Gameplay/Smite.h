

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Characters/Prototype2Character.h"


class PROTOTYPE2_API Smite
{
public:
	Smite();
	~Smite();

	void Tick(float DeltaTime);

	static constexpr float TimerStartTime{90};
	static constexpr float StrikeTimeIncrease{30};
	float Timer{0};

	APrototype2Character* Player{nullptr};
	UWeaponData* SmiteData;

	void IncreaseTime(float _Amount);
	void SetSmiteTime(float Amount);
	void Strike();

	void SetPlayer(APrototype2Character* _Player);
	
};
