#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGamestate.generated.h"


UCLASS()
class PROTOTYPE2_API ALobbyGamestate : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALobbyGamestate();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldServerTravel{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int LobbyLengthMinutes{0};
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int LobbyLengthSeconds{30};

private:
	void CountdownLobbyTimer();
	
	bool PreviousServerTravel{};
	
	FTimerHandle LobbyTimerHandle;
};
