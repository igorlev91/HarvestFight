

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

	/* Toggle ingame menu on/off */
	UFUNCTION(BlueprintCallable)
	void EnableDisableMenu();

	/* Toggle radial emote on/off */
	UFUNCTION(BlueprintCallable)
	void ShowEmoteRadialMenu();

	UFUNCTION(BlueprintCallable)
	void DisableEmoteRadialMenu();

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
	class UWidget_EmoteRadial* EmoteRadialMenu;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_GameOptions* OptionsMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_EndgameMenu* EndgameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEndgame{false};
	
	/* Timer */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Minutes;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Seconds;

	/* Solo Player Games */
	/* Player UI scores */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player1Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player1ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player2Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player2ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player3Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player3ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player4Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player4ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player5Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player5ExtraCoins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player6Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Player6ExtraCoins;

	/* Player Icons */
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> PlayerIcons{{}, {}, {}, {}, {}, {}};
	
	/* Array of all icons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> Icons;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P1Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P2Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P3Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P4Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P5Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P6Icon;

	/* Icon rings */
	UPROPERTY(EditAnywhere)
	class UTexture2D* RingTexture{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P1Ring;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P2Ring;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P3Ring;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P4Ring;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P5Ring;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* P6Ring;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> Rings;

	/* Array of overlays */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UOverlay*> Overlays;
	
	// Player UI overlays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* TopOverlayUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayPlayer1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayPlayer2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayPlayer3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayPlayer4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayPlayer5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayPlayer6;

	/* Array of all player names */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UTextBlock*> PlayerNames;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* P1Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* P2Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* P3Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* P4Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* P5Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* P6Name;

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
	UPROPERTY(EditAnywhere)
	class UColourData* ColourData;
	
	/* Team UI scores */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team1Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team1ExtraCoinsP1;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team1ExtraCoinsP2;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team1ExtraCoinsP3;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team2Coins;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team2ExtraCoinsP1;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team2ExtraCoinsP2;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* Team2ExtraCoinsP3;

	/* Team overlays */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* TeamsOverlay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayTeam1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayTeam2;

	/* Team rings/colours */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* T1Ring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* T2Ring;
	
	// Pickup UI
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* PickupImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* OverlayPickup;

	IInteractInterface* lastInteract{};

	// Weapon UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* WeaponImage;
	
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
	UTextBlock* InteractionText; 
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* InteractionButtonImage;
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

	/* Changing player name background HUD if black text */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	bool bHasChangedBlackTextOutlineColour{false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	int32 PlayerIDToChangeOutline{};
};
