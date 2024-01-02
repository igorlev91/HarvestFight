// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_LobbyPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_LobbyPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* ReadyButton;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* CancelButton;

	class ALobbyGamestate* GameStateRef;

	// Player text -ready or not
	// Player 1
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player1Text;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player1ReadyText;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* Player1ReadyImage;
	// Player 2
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player2Text;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player2ReadyText;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* Player2ReadyImage;
	// Player 3
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player3Text;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player3ReadyText;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* Player3ReadyImage;
	// Player 4
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player4Text;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player4ReadyText;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* Player4ReadyImage;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* StartCountDown;
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void SetReady();

	UFUNCTION(BlueprintCallable)
	void SetCancel();
};
