#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Prototype2Gamestate.generated.h"

UCLASS()
class PROTOTYPE2_API APrototype2Gamestate : public AGameStateBase
{
	GENERATED_BODY()

public:
	APrototype2Gamestate();
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldServerTravel{true};

	UPROPERTY(EditAnywhere)
	int MatchLengthMinutes{5};
	UPROPERTY(EditAnywhere)
	int MatchLengthSeconds{0};
	
private:
	bool PreviousServerTravel{};


	FTimerHandle MatchTimerHandle;
public:
	void CountdownMatchTimer();
};
