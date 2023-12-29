// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerState.h"

#include "Net/UnrealNetwork.h"

void APrototype2PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2PlayerState, Coins);
}
