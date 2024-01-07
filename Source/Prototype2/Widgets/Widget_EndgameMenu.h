

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_EndgameMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_EndgameMenu : public UUserWidget
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

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* TextGameWinnerPlayer;

	class APrototype2Gamestate* GameStateReference;

	

	
};
