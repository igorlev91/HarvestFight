// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_StartAndEndMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_StartAndEndMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget)) 
	class UTextBlock* TimerText;

	UPROPERTY(VisibleAnywhere) 
	class APrototype2Gamestate* GameStateRef{nullptr};

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnInitialized() override;
	void UpdateTimerText();
	void CheckForGameFinished();
};
