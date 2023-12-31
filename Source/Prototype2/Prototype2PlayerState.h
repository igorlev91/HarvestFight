// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
public:
	UPROPERTY(Replicated, VisibleAnywhere)
    int Player_ID{};
	
	UPROPERTY(Replicated, EditAnywhere)
	int Coins{5};
};
