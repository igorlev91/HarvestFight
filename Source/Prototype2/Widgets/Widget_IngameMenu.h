// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_IngameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_IngameMenu : public UUserWidget
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	void EnableDisableMenu();

	// Turns off ingame menu if visible
	UFUNCTION(BlueprintCallable)
	void DisableMenu();
	
	UFUNCTION(BlueprintCallable)
	void ReturnToMenu();
};
