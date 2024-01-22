

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/InteractInterface.h"
#include "Widget_PlayerHUD.generated.h"

USTRUCT(BlueprintType)
struct FEmphasizer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	class UImage* Image = nullptr;
	UPROPERTY(BlueprintReadWrite)
	FVector2D OriginalScale = {1.0f, 1.0f};
	UPROPERTY(BlueprintReadWrite)
	FVector2D DesiredScale = {2.0f, 2.0f};
	UPROPERTY(BlueprintReadWrite)
	float Speed = 2.0f;
	UPROPERTY(BlueprintReadWrite)
	bool bHasEmphasized = false;

	// == operator overloaded to be able to remove from array
	bool operator==(FEmphasizer const &B) const
	{
		return (this->Image == B.Image &&
				this->OriginalScale == B.OriginalScale &&
				this->DesiredScale == B.DesiredScale &&
				this->Speed == B.Speed &&
				this->bHasEmphasized == B.bHasEmphasized);
	}
};

class UPlantData;

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

	/* Updates the player icons & colour - top of screen for each player */
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerIcons();

	void UpdateEmphasizers(float _DeltaTime);
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FEmphasizer> Emphasizers;
	
	/* Widgets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_IngameMenu* IngameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_EndgameMenu* EndgameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEndgame{false};
	
	/* Timer */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Minutes;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Seconds;

	/* Solo Player Games */
	/* Player UI scores */
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
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player5Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player5ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player6Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Player6ExtraCoins;

	/* Player Icons */
	UPROPERTY(EditAnywhere)
	TArray<class UTexture2D*> PlayerIcons{{}, {}, {}, {}, {}, {}};
	
	/* Array of all icons */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Icons;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P1Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P2Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P3Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P4Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P5Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P6Icon;

	/* Icon rings */
	UPROPERTY(EditAnywhere)
	class UTexture2D* RingTexture{};
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P1Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P2Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P3Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P4Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P5Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* P6Ring;

	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Rings;

	// Player UI overlays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* TopOverlayUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayPlayer1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayPlayer2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayPlayer3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayPlayer4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayPlayer5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayPlayer6;

	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Crowns;
	
	/* Player rings */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Crown;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Crown;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Crown;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Crown;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Crown;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Crown;

	/* Team games */
	/* Team UI scores */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Team1Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Team1ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Team2Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Team2ExtraCoins;

	/* Team overlays */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayTeam1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UOverlay* OverlayTeam2;
	
	// Pickup UI
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PickupImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UOverlay* OverlayPickup;

	IInteractInterface* lastInteract{};

	// Weapon UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* WeaponImage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* LeekWeaponTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* AsparagusWeaponTexture;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* WeaponSeedTexture;

	//// Object Textures
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* CarrotTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* CarrotGoldTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* CarrotSeedTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* CabbageTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* CabbageGoldTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* CabbageSeedTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* MandrakeTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* MandrakeGoldTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* MandrakeSeedTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* BroccoliTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* BroccoliGoldTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* BroccoliSeedTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* DaikonTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* DaikonGoldTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* DaikonSeedTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* RadishTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* RadishGoldTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UTexture2D* RadishSeedTexture;
	
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
	
};
