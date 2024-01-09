.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()
	/* Public Functions */
public:
	/* Constructor */
	ALobbyGamemode();

	/* Public Variables */
public:
	/* Player Z Positions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerZPosition = 90.0f;

private:
	/* Called after logging in */
	virtual void PostLogin(APlayerController* _NewPlayer) override;

	/* Called when logging out */
	virtual void Logout(AController* _Exiting) override;

	/* Your classic tick, tried and true */
	virtual void Tick(float _DeltaSeconds) override;

private:
	/* For holding all the costumes */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};

	/* Player 1 character lobby starting positions */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector Player1StartPosition = FVector{2000.0f,40.0f, PlayerZPosition};

	/* Player distances in lobby */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float DistanceBetweenPlayers = 230.0f;
};
