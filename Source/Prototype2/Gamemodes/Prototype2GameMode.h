

#pragma once

#include "CoreMinimal.h"
#include "HHGamemode.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2/Gameplay/Endgame/EndGamePodium.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/Gameplay/Crown.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2GameMode.generated.h"

UCLASS(minimalapi)
class APrototype2GameMode : public AHHGamemodeBase
{
	GENERATED_BODY()

	friend ACrown;
	friend APrototype2Gamestate;

	/* Public Variables */
public:
	class AEndGamePodium* GetEndGamePodium();
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};
	
	UPROPERTY(EditAnywhere)
	TArray<USkeletalMesh*> PlayerMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URandomEventManager* RandomEventManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTeams{};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	class ASellBin* SellBinRef;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class AEndGamePodium* EndGamePodium{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<class APreGameArena*> PreGameArenas{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	APreGameArena* DefaultPreGameArena{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class ADataAssetWorldOverride* DataAssetWorldOverride{};
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class ACrown* KingCrown{};
	
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
	void Multi_AssignCharacterSkin(APrototype2PlayerState* _CastedPlayerState, APrototype2Gamestate* _GameStateReference, UPrototypeGameInstance* _gameInstanceReference, const FString& _NewName);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_TeleportEveryoneToPodium();
	void Multi_TeleportEveryoneToPodium_Implementation();

	void KeepPlayersAtSpawnPositionUntilStart();
	void PupeteerPlayerCharactersForEndGame();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PupeteerPlayerCharactersForEndGame(APrototype2Character* _Target);

	void UpdateAllPlayerInfo(class APrototype2Gamestate* _GameStateReference, class UPrototypeGameInstance* _gameInstanceReference);
	
	void SpawnTeamsPreGameArena();
	void TeleportHostToPreGameArena();
	void TeleportHostToTeamsPreGameArena();
	void ColourTeamsPreGameArenas();

	void TeleportToPreGameArena(APrototype2Character* _Player);
	void TeleportUnteleportedPlayersToPreGameArena_Teams();

	/* Protected Variables */
protected:
	void SpawnDefaultPregameArena();
	
	UPROPERTY(VisibleAnywhere)
	bool bHasSetTeamsColours{};
	UPROPERTY(VisibleAnywhere)
	bool bHostHasTpdToPreGameArena{};
	UPROPERTY(VisibleAnywhere)
	bool bHostHasTpdToTeamsPreGameArena{};
	UPROPERTY(VisibleAnywhere)
	bool bPreGameArenasAdjustedForTeams{};
	
	bool bHasGameFinishedLocal{};
	bool bTpHasHappened{};
	UPROPERTY(EditAnywhere)
	float AutomaticCrownCheckFrequency{1.0f};
	UPROPERTY()
	float AutomaticCrownCheckTimer{};

	UPROPERTY()
	TArray<bool> PlayersTpdToPreGameArena_Teams{};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess)) 
	class APrototype2Gamestate* GameStateRef{nullptr};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<AEndGamePodium> EndGamePodiumPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<ASellBin> SellBinPrefab;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<APreGameArena> PreGameArenaPrefab;

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

	UPROPERTY(EditAnywhere)
	UColourData* ColourData;
};



