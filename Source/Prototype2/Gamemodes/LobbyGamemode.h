#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"


UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()

	ALobbyGamemode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};


	// Player character lobby positions
	FVector Position1{567.4f,-143.0f,97.0f}; // Left position for 4 player
	FVector Position2{446.0f, -187.2f, 97.0f}; // Left position for 3 player
	FVector Position3{324.6f,-231.4f,97.0f}; // Left position for 2 player
	FVector Position4{212.2f, -272.3f, 97.0f}; // Centre position for 3 player
	FVector Position5{99.8f,-313.2f,97.0f};  // Right position for 2 player
	FVector Position6{-11.5, -353.7f, 97.0f}; // Right position for 3 player
	FVector Position7{-122.7f,-394.2f,97.0f}; // Right position for 4 player
};
