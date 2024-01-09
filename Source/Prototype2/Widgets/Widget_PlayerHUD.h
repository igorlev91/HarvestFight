

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/InteractInterface.h"
#include "Widget_PlayerHUD.generated.h"

class UPlantData;
/* Enum for items that can be picked up */
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
	WeaponSeed,
	NoWeapon
};

UCLASS()
class PROTOTYPE2_API UWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	class APrototype2Gamestate* GameStateReference;

	/* Turns ingame menu on/off */
	UFUNCTION(BlueprintCallable)
	void EnableDisableMenu();

	/* Enables the endgame menu */
	UFUNCTION(BlueprintCallable)
	void EnableEndgameMenu();

	/* Update Pickup UI with Texture passed in */
	void UpdatePickupUI(UTexture2D* _PickupTexture);
	
	/* Clears the pickup UI slot in the HUD */
	void ClearPickupUI();

	/* Changes the text for interaction eg) Pickup, Sell, Grow */
	UFUNCTION(BlueprintCallable)
	void SetHUDInteractText(FString _InteractionText);

	/* Makes the interaction image pulse eg) 'E' */
	UFUNCTION(BlueprintCallable)
	void InteractionImagePulse(float _DeltaTime);

	// Probably can delete
	UFUNCTION(BlueprintCallable)
	void SetPlayerSprintTimer(float _SprintTime);

	// probably can delete
	UFUNCTION(BlueprintCallable)
	void SetWeaponDurability(int32 _Durability);

	/* Sets the player icons - top of screen for each player */
	UFUNCTION(BlueprintCallable)
	void SetPlayerIcons(int32 _IconNumber, APrototype2PlayerState* _Player);

	/* Sets the player icon based on SetPlayerIcons function */
	UFUNCTION(BlueprintCallable)
	UTexture2D* SetIcon(APrototype2PlayerState* _Player);
	
public:
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

	UPROPERTY(EditAnywhere)
	class UTexture2D* RingTexture{};
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P1ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P2ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P3ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P4ring;

	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Rings;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* LeekWeaponTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AsparagusWeaponTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WeaponSeedTexture;

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
	
	/* Interaction image and text - eg "Grow" */
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

	/* Player Icon Textures */
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
};





