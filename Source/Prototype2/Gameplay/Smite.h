

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Smite.generated.h"

UCLASS()
class PROTOTYPE2_API USmite : public UObject
{

	GENERATED_BODY()
	
public:
	USmite();
	~USmite();

	void Tick(float DeltaTime);

	UPROPERTY(BlueprintReadOnly)
	float TimerStartTime{90};
	inline static float StrikeTimeIncrease{30};
	UPROPERTY(BlueprintReadOnly)
	float Timer{0};

	APrototype2Character* Player{nullptr};
	UWeaponData* SmiteData;

	void IncreaseTime(float _Amount);
	void SetSmiteTime(float Amount);
	void Strike();

	void SetPlayer(APrototype2Character* _Player);
	
};
