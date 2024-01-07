
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()
public:
	/* Constructor */
	ALobbyGamemode();

private:
	/* Called after loging in */
	virtual void PostLogin(APlayerController* _NewPlayer) override;

	/* Called when logging out */
	virtual void Logout(AController* _Exiting) override;

	/* Your classic tick, tried and true */
	virtual void Tick(float _DeltaSeconds) override;

private:
	/* For holding all the costumes */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};

	// Player character lobby magical positions
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<FVector> PlayerPositions
	{
		FVector{567.4f,-143.0f,97.0f}, // Left position for 4 player
		FVector{446.0f, -187.2f, 97.0f}, // Left position for 3 player
		FVector{324.6f,-231.4f,97.0f}, // Left position for 2 player
		FVector{212.2f, -272.3f, 97.0f}, // Centre position for 3 player
		FVector{99.8f,-313.2f,97.0f},  // Right position for 2 player
		FVector{-11.5, -353.7f, 97.0f}, // Right position for 3 player
		FVector{-122.7f,-394.2f,97.0f} // Right position for 4 player
	};
};
