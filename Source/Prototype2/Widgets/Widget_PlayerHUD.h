// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerHUD.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_InteractionPanel* InteractionPanel;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_IngameMenu* IngameMenu;

	// Timer
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Seconds;

	// Player scores
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Coins;
	
public:
	// Functions
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	class APrototype2Gamestate* GameStateRef;
	
	UFUNCTION(BlueprintCallable)
	void EnableDisableMenu();
};
