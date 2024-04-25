

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Gameplay/Endgame/EndGamePodium.h"
#include "Prototype2PlayerController.generated.h"

UCLASS()
class PROTOTYPE2_API APrototype2PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/* Constructor */
	virtual void BeginPlay() override;

	/* Tick */
	virtual void Tick(float DeltaSeconds) override;

	/* Sets the player to ready in lobby */
	void SetIsReady(int32 _Player, bool _bIsReady);

	/* When a player votes for a map */
	void VoteMap(EFarm _Level);

	/* Update characters material when they change the costume */
	void UpdateCharacterMaterial(int32 _Player, FCharacterDetails _Details);

	/* Sends player back to menu */
	UFUNCTION(BlueprintCallable)
	void KickFromLobby();

	/* Public Variables */
	UPROPERTY(VisibleAnywhere) 
	class APrototype2Gamestate* GameStateRef{nullptr};
	bool bEnableMovement{false};
	
	UPROPERTY(VisibleAnywhere)
	float CameraBlendTimer{};

	void SetViewTarget_Networked(AActor* _ViewTarget);
	UFUNCTION(Server, Reliable)
	void Server_SetViewTarget_Networked(AActor* _ViewTarget);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetViewTarget_Networked(AActor* _ViewTarget);
	
	/* RPC for setting player ready in lobby */
	UFUNCTION(Server, Reliable)
	void Server_SetIsReady(int32 _Player, bool _bIsReady);
	/* RPC for voting for a map */
	UFUNCTION(Server, Reliable)
	void Server_VoteMap(EFarm _Level);

	/* Multicast that updates the characters costume */
	UFUNCTION(Server, Reliable)
	void Server_UpdateCharacterMaterial(int32 _Player, FCharacterDetails _Details);
	
	void SyncPlayerMaterial(int32 _PlayerID, FCharacterDetails _CharacterDetails);

	UFUNCTION(Server, Reliable)
	void Server_UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails, const FString& _PlayerName);
};
