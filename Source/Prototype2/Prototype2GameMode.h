// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2/EndGamePodium.h"
#include "Prototype2/Prototype2Character.h"
#include "Prototype2/Prototype2PlayerState.h"
#include "Prototype2GameMode.generated.h"

UCLASS(minimalapi)
class APrototype2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrototype2GameMode();
	
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void Tick(float DeltaSeconds) override;

	void DisableControllerInput(APlayerController* PlayerController);
	void EnableControllerInput(APlayerController* PlayerController);
	void DisableControllerInputForAll();
	void EnableControllerInputForAll();

	void LookOutForGameEnd();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_TeleportEveryoneToPodium();
	void Multi_TeleportEveryoneToPodium_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DetachShippingBinComponents();
	void Multi_DetachShippingBinComponents_Implementation();
	
	bool HasGameFinishedLocal{};
	bool TpHasHappened{};
	
	UPROPERTY(Replicated, VisibleAnywhere)
	class ASellBin* SellBinRef;

	UPROPERTY(VisibleAnywhere) 
	class APrototype2Gamestate* GameStateRef{nullptr};

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEndGamePodium> EndGamePodiumPrefab;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ASellBin> SellBinPrefab;
	
	UPROPERTY(Replicated, EditAnywhere)
	class AEndGamePodium* EndGamePodium{};
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};
	
	UPROPERTY(EditAnywhere)
	TArray<USkeletalMesh*> PlayerMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<APrototype2Character>> Server_Characters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<APrototype2PlayerState>> Server_PlayerStates;
};



