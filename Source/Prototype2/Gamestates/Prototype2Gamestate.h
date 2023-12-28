#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Prototype2Gamestate.generated.h"

UCLASS()
class PROTOTYPE2_API APrototype2Gamestate : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	// Timer
	UPROPERTY(EditAnywhere)
	int Minutes{5};

	UPROPERTY(EditAnywhere)
	int Seconds;

public:
	void Countdown();
};
