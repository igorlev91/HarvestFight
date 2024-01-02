// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Prototype2/PrototypeGameInstance.h"
#include "Prototype2PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API APrototype2PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void SetIsReady(int _player, bool _isReady);
	
	UFUNCTION(Server, Reliable)
	void Server_SetIsReady(int _player, bool _isReady);
	void Server_SetIsReady_Implementation(int _player, bool _isReady);

	void VoteMap(int _player, EFarm _level);
	
	UFUNCTION(Server, Reliable)
	void Server_VoteMap(int _player, EFarm _level);
	void Server_VoteMap_Implementation(int _player, EFarm _level);

	void UpdateCharacterMaterial(int _player, ECharacters _character, ECharacterColours _characterColour);

	UFUNCTION(Server, Reliable)
	void Server_UpdateCharacterMaterial(int _player, ECharacters _character, ECharacterColours _characterColour);
	void Server_UpdateCharacterMaterial_Implementation(int _player, ECharacters _character, ECharacterColours _characterColour);
	
	UFUNCTION(BlueprintCallable)
	void KickFromLobby();

	UPROPERTY(VisibleAnywhere) 
	class APrototype2Gamestate* GameStateRef{nullptr};
};
