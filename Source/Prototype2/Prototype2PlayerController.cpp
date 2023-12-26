// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Widget_PlayerHUD.h"

void APrototype2PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerHudPrefab)
	{
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(this, PlayerHudPrefab);
		PlayerHUDRef->AddToViewport();
	}

	
}
