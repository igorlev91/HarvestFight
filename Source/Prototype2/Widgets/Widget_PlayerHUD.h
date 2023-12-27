// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/Prototype2GameMode.h"
#include "Widget_PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_InteractionPanel* InteractionPanel;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_IngameMenu* IngameMenu;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Seconds;

	
public:
	// Functions
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	APrototype2GameMode* GameModeRef;
	
	UFUNCTION(BlueprintCallable)
	void EnableDisableMenu();
};
