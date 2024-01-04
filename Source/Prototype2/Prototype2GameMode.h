// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2GameMode.generated.h"

UCLASS(minimalapi)
class APrototype2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrototype2GameMode();
	
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;

	void DisableControllerInput(APlayerController* PlayerController);
	void EnableControllerInput(APlayerController* PlayerController);
	void DisableControllerInputForAll();
	void EnableControllerInputForAll();

	void LookOutForGameEnd();

	bool HasGameFinishedLocal{};

	UPROPERTY(VisibleAnywhere) 
	class APrototype2Gamestate* GameStateRef{nullptr};

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEndGamePodium> EndGamePodiumPrefab;
	
	UPROPERTY(EditAnywhere)
	class AEndGamePodium* EndGamePodium{};
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};
};



