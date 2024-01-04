// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/InteractInterface.h"
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
	MandrakeSeed,
	Broccoli,
	BroccoliSeed,
	Daikon,
	DaikonSeed,
	Radish,
	RadishSeed,
	Weapon,
	NoWeapon
};

UCLASS()
class PROTOTYPE2_API UWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	//UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	//class UWidget_InteractionPanel* InteractionPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_IngameMenu* IngameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_EndgameMenu* EndgameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEndgame{false};

	// Timer
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Seconds;

	// Player scores
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player1Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player1ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player2Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player2ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player3Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player3ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player4Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player4ExtraCoins;

	UPROPERTY(EditAnywhere)
	TArray<class UTexture2D*> PlayerIcons{{},{},{},{}, {}, {}, {}, {}};
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P1Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P2Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P3Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P4Icon;

	// Player UI overlays
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* TopOverlayUI;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* Overlay_P1;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* Overlay_P2;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* Overlay_P3;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* Overlay_P4;
	
	// Pickup UI
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PickupImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* OverlayPickup;

	IInteractInterface* lastInteract{};

	// Weapon UI
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* WeaponImage;

	// Object Textures
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CarrotTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CarrotGoldTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CarrotSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CabbageTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CabbageGoldTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CabbageSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* MandrakeTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* MandrakeGoldTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* MandrakeSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BroccoliTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BroccoliGoldTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BroccoliSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DaikonTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DaikonGoldTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DaikonSeedTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* RadishTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* RadishGoldTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* RadishSeedTexture;
	
	// Interaction image and text
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	class UTextBlock* InteractionText; 
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* InteractionButtonImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UHorizontalBox* InteractionUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ETexture1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ETexture2;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	bool bInteractionButtonShowing{false};
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	bool bShowETexture1{true};
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	float interactionButtonMaxTime{0.2f};
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	float interactionButtonTimer{};
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UWidget_StartAndEndMenu* StartAndEndMenu;

	// Player Icon Textures
	// Cow
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Cow_White_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Cow_Red_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Cow_Blue_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Cow_Green_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Cow_Yellow_Texture;
	// Pig
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Pig_White_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Pig_Red_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Pig_Blue_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Pig_Green_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Pig_Yellow_Texture;
	// Chicken
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Chicken_White_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Chicken_Red_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Chicken_Blue_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Chicken_Green_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Chicken_Yellow_Texture;
	// Duck
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Duck_White_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Duck_Red_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Duck_Blue_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Duck_Green_Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Duck_Yellow_Texture;
	
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
	void UpdatePickupUI(EPickup _pickup, bool _isGold);
	
	UFUNCTION(BlueprintCallable)
	void UpdateWeaponUI(EPickup _weapon);

	UFUNCTION(BlueprintCallable)
	void SetHUDInteractText(FString _interactionText);

	UFUNCTION(BlueprintCallable)
	void InteractionImagePulse(float _dt);

	UFUNCTION(BlueprintCallable)
	void SetPlayerSprintTimer(float _sprintTime);

	UFUNCTION(BlueprintCallable)
	void SetWeaponDurability(int _durability);

	UFUNCTION(BlueprintCallable)
	void SetPlayerIcons(int _iconNum, APrototype2PlayerState* _player);

	UFUNCTION(BlueprintCallable)
	UTexture2D* SetIcon(APrototype2PlayerState* _player);
};





