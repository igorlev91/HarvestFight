// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_EndgameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_EndgameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* GameWinnerText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* GameWinnerPoints;

	class APrototype2Gamestate* GameStateRef;

	
public:
	// Functions
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateWinnerText();

	void EnableEndgameMenu();
	
};
