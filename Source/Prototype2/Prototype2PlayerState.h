// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrototypeGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Prototype2PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API APrototype2PlayerState : public APlayerState
{
	GENERATED_BODY()

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	APrototype2PlayerState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	public:
	void GrabSkinFromGameInstance();
	
	UFUNCTION(Server, Reliable)
	void Server_GrabSkinFromGameInstance(ECharacters _character, ECharacterColours _colour);
	void Server_GrabSkinFromGameInstance_Implementation(ECharacters _character, ECharacterColours _colour);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GrabSkinFromGameInstance(ECharacters _character, ECharacterColours _colour);
	void Multi_GrabSkinFromGameInstance_Implementation(ECharacters _character, ECharacterColours _colour);

	
	
public:
	bool SkinSet{};
	
	UPROPERTY(Replicated, VisibleAnywhere)
    int Player_ID{};
	
	UPROPERTY(Replicated, EditAnywhere)
	int Coins{0};

	// Showing coins that are being added to total
	UPROPERTY(Replicated, EditAnywhere)
	int ExtraCoins{0};
	UPROPERTY(Replicated, EditAnywhere)
	bool IsShowingExtraCoins{false};
	UPROPERTY(Replicated, EditAnywhere)
	float MaxTimeShowExtraCoins{};
	UPROPERTY(Replicated, VisibleAnywhere)
	float TimerExtraCoins{};

	void UpdateCharacterMaterial(ECharacters _character, ECharacterColours _characterColour);

	// Character & Colour
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ECharacters Character{ECharacters::COW};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ECharacterColours CharacterColour{ECharacterColours::RED};
	
};
