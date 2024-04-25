

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
#include "Widget_EndgameMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_EndgameMenu : public UUserWidget, public IWidgetUtilityInterface
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	/*  Updates the endgame text in the HUD showing who the winner is */
	void UpdateWinnerText();

	/* Turns on the endgame menu widget */
	void EnableEndgameMenu();

	/* Turns on the endgame menu widget */
	void EnableEndgameMapChoice();
	
	/* Public Variables */
public:
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* TextGameWinner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APrototype2Gamestate* GameStateReference;

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

	/* Classic Mode Map Choice Widget */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_MapChoice* WBP_MapChoice;
	
	/* Brawl Mode Map Choice Widget*/
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoiceBrawl;

	/* Blitz Mode Map Choice Widget*/
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoiceBlitz;
	
};
