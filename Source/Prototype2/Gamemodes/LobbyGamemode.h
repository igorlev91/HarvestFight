#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"


UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()

	
public:

	ALobbyGamemode();

private:
	virtual void PostLogin(APlayerController* _NewPlayer) override;
	void Logout(AController* _Exiting) override;

	virtual void Tick(float _DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};

	// Player character lobby positions
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<FVector> PlayerPositions
	{
		{567.4f,-143.0f,97.0f}, // Left position for 4 player
		{446.0f, -187.2f, 97.0f}, // Left position for 3 player
		{324.6f,-231.4f,97.0f}, // Left position for 2 player
		{212.2f, -272.3f, 97.0f}, // Centre position for 3 player
		{99.8f,-313.2f,97.0f},  // Right position for 2 player
		{-11.5, -353.7f, 97.0f}, // Right position for 3 player
		{-122.7f,-394.2f,97.0f} // Right position for 4 player
	};
};
