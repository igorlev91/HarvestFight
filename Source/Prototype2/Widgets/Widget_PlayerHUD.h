// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerHUD.generated.h"

UENUM(BlueprintType)
enum EPickup
{
	None,
	Carrot,
	CarrotSeed,
	Cabbage,
	CabbageSeed,
	Mandrake,
	MandrakeSeed
};

UCLASS()
class PROTOTYPE2_API UWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	//UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	//class UWidget_InteractionPanel* InteractionPanel;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_IngameMenu* IngameMenu;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_EndgameMenu* EndgameMenu;

	// Timer
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Seconds;

	// Player scores
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player1Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player2Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player3Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player4Coins;

	// Weapon UI
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* WeaponImage;

	// Pickup UI
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PickupImage;
	// Textures
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CarrotTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CarrotSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CabbageTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CabbageSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* MandrakeTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* MandrakeSeedTexture;

	// Interaction Text
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* InteractionText;
	
public:
	// Functions
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	class APrototype2Gamestate* GameStateRef;
	
	UFUNCTION(BlueprintCallable)
	void EnableDisableMenu();

	UFUNCTION(BlueprintCallable)
	void EnableEndgameMenu();

	//UFUNCTION(BlueprintCallable)
	//void UpdateWeaponUI();

	UFUNCTION(BlueprintCallable)
	void UpdatePickupUI(EPickup _pickup);

	UFUNCTION(BlueprintCallable)
	void SetHUDInteractText(FString _interactionText);
};
