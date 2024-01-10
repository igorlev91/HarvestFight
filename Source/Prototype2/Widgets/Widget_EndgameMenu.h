

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

	/* Public Variables */
public:
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* TextGameWinner;

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

	
};
