

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Widget_LobbyPlayerHUDV2.generated.h"

enum EGameSpeed : int;

UCLASS()
class PROTOTYPE2_API UWidget_LobbyPlayerHUDV2 : public UUserWidget, public IWidgetUtilityInterface
{
	GENERATED_BODY()
	
	/* Public Functions */
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* Updates playerstate to being ready & turns on ready UI*/
	UFUNCTION(BlueprintCallable)
	void SetReady();

	/* Updates playerstate to being not-ready & turns off ready UI */
	UFUNCTION(BlueprintCallable)
	void SetCancel();
	
	void UpdateMapChoice(class UWidget_MapChoice* _MapChoiceWidget);
	void UpdateMapChoiceTimer(UWidget_MapChoice* _MapChoiceWidget);

	UFUNCTION()
	void SetOwningController(class ALobbyPlayerController* _Owner);

	UFUNCTION()
	void InitTeams();

	UFUNCTION()
	void UpdateTeams();

	/* Public Variables */
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ALobbyPlayerState* OwningPlayerState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category = "Ready Button")
	UButton* ReadyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category = "Cancel Button")
	UButton* CancelButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ALobbyGamestate* GameStateReference;
	
	/* Classic Mode Map Choice Widget */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoice;
	
	/* Brawl Mode Map Choice Widget*/
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoiceBrawl;

	/* Blitz Mode Map Choice Widget*/
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoiceBlitz;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UWidget_LobbyCharacterSelection* WBP_LobbyCharacterSelection;

	// teams
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTeams{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget)) 
	class UTextBlock* TeamAText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget)) 
	UTextBlock* TeamBText;

	UPROPERTY(EditAnywhere)
	class UColourData* ColourData;
	
	/* Icon rings */
	UPROPERTY(EditAnywhere)
	UTexture2D* RingTexture{};

	/* Vertical Boxes - holding players overlays */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UVerticalBox* VerticalBoxLeft;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UVerticalBox* VerticalBoxRight;
	
	/* Array of all overlays */
	UPROPERTY(VisibleAnywhere)
	TArray<class UOverlay*> Overlays;
	
	/* Player Icons */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer1;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer2;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer3;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer4;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer5;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer6;
	
	/* Array of all icons */
	UPROPERTY(VisibleAnywhere)
	TArray<class UImage*> Icons;
	
	/* Player Icons */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Icon;
	
	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Rings;
	
	/* Player rings */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Ring;

	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Backgrounds;
	
	/* Player rings */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Background;

	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<class UTextBlock*> Names;
	
	/* Players names */
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

	/* Array of player ready images */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> ReadyImages;

	/* Player ready images */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6ReadyImage;

	/* Team borders */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* LeftTeamOverlay;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* RightTeamOverlay;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* BackgroundImageT1;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* BackgroundImageT2;

	/* Array of 'you' images */
	UPROPERTY(VisibleAnywhere)
	TArray<UOverlay*> YouOverlays;

	/* Player ready images */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P1YouOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P2YouOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P3YouOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P4YouOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P5YouOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P6YouOverlay;

	/* Array of 'you' images */
	UPROPERTY(VisibleAnywhere)
	TArray<UOverlay*> KickOverlays;

	/* Player ready images */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P1KickOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P2KickOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P3KickOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P4KickOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P5KickOverlay;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UOverlay* P6KickOverlay;
	


	
	/* Widget Variables related to button pulsing/size changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	bool bIsButtonHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	int32 ButtonIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	FVector2D ButtonScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	class UButton* ButtonToPulse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	float PulseTime;



	/* Host Options */
	// Reset settings to defaults
	UFUNCTION(BlueprintCallable)
	void ResetDefaults();

	// Reset settings to actual setting - non temp
	UFUNCTION(BlueprintCallable)
	void ResetSetting();

	// Confirm settings 
	UFUNCTION(BlueprintCallable)
	void ConfirmSetting();
	
	// Game Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_OptionSelector* GameMode_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DefaultHHGameMode{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TempHHGameMode{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HHGameMode{0};

	UFUNCTION()
	void OnGameModeControlLeftButtonPressed();
	UFUNCTION()
	void OnGameModeControlRightButtonPressed();
	UFUNCTION(BlueprintCallable)
	void UpdateGameModeText();
	UFUNCTION(BlueprintCallable)
	void SetGameModeControl();
	
	// Game Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_OptionSelector* GameLength_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> DefaultGameSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> TempGameSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> GameSpeed;

	UFUNCTION()
	void OnGameSpeedControlLeftButtonPressed();
	UFUNCTION()
	void OnGameSpeedControlRightButtonPressed();
	UFUNCTION(BlueprintCallable)
	void UpdateGameSpeedText();
	UFUNCTION(BlueprintCallable)
	void SetGameSpeedControl();

	// Stealing - Whether players can steal from other plots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Stealing_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultStealingSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempStealingSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool StealingSetting = true;

	UFUNCTION()
	void OnStealingControlButtonPressed();
	UFUNCTION(BlueprintCallable)
	void UpdateStealingText();
	UFUNCTION(BlueprintCallable)
	void SetStealingControl();

	// Fertiliser - Whether fertiliser spawns for maps that have it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Fertiliser_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultFertiliserSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempFertiliserSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FertiliserSetting = true;

	UFUNCTION()
	void OnFertiliserControlButtonPressed();
	UFUNCTION(BlueprintCallable)
	void UpdateFertiliserText();
	UFUNCTION(BlueprintCallable)
	void SetFertiliserControl();

	// Cement - Whether cement spawns for maps that have it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Cement_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultCementSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempCementSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CementSetting = true;

	UFUNCTION()
	void OnCementControlButtonPressed();
	UFUNCTION(BlueprintCallable)
	void UpdateCementText();
	UFUNCTION(BlueprintCallable)
	void SetCementControl();
};
