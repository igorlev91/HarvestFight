#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"


UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()

	ALobbyGamemode();
public:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	bool ShouldServerTravel{};

private:
	bool PreviousServerTravel{};
};
