

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2/Gameplay/Endgame/EndGamePodium.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2GameMode.generated.h"

UCLASS(minimalapi)
class APrototype2GameMode : public AGameModeBase
{
	GENERATED_BODY()

	/* Public Variables */
public:
	class AEndGamePodium* GetEndGamePodium();
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};
	
	UPROPERTY(EditAnywhere)
	TArray<USkeletalMesh*> PlayerMeshes;

	/* Protected Functions */
protected:
	APrototype2GameMode();
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	virtual void PostLogin(APlayerController* _NewPlayer) override;
	virtual void Logout(AController* _Exiting) override;
	virtual void Tick(float _DeltaSeconds) override;

	void DisableControllerInput(APlayerController* _PlayerController);
	void EnableControllerInput(APlayerController* _PlayerController);
	void DisableControllerInputForAll();
	void EnableControllerInputForAll();

	void LookOutForGameEnd();

	void TeleportEveryoneToPodium();

	/* Networking */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_AssignCharacterSkin(APrototype2Character* _Target, int32 _CharacterSelection);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_TeleportEveryoneToPodium();
	void Multi_TeleportEveryoneToPodium_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DetachShippingBinComponents();
	void Multi_DetachShippingBinComponents_Implementation();

	void KeepPlayersAtSpawnPositionUntilStart();
	void PupeteerPlayerCharactersForEndGame();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PupeteerPlayerCharactersForEndGame(APrototype2Character* _Target);

	/* Protected Variables */
protected:
	bool bHasGameFinishedLocal{};
	bool bTpHasHappened{};
	
	UPROPERTY(Replicated, VisibleAnywhere, meta = (AllowPrivateAccess))
	class ASellBin* SellBinRef;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess)) 
	class APrototype2Gamestate* GameStateRef{nullptr};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<AEndGamePodium> EndGamePodiumPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<ASellBin> SellBinPrefab;
	
	UPROPERTY(Replicated, EditAnywhere, meta = (AllowPrivateAccess))
	class AEndGamePodium* EndGamePodium{};

	/* Player start positions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category="Player Start Position")
	float PlayerStartingZPosition{90.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess), Category="Player Start Position")
	float PlayerStartingDistanceTowardsSellBin{300.0f};

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimationData*> AnimationDatas; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TArray<TObjectPtr<APrototype2Character>> Server_Characters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TArray<TObjectPtr<APrototype2PlayerState>> Server_PlayerStates;
};



