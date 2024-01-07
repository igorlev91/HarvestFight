

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_StartAndEndMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_StartAndEndMenu : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/* Updates the text for the timer */
	void UpdateTimerText();

	/* Checks if game has finished, & if true, turns on "Times Up" text */
	void CheckForGameFinished();
	
	/* Public Variables */
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget)) 
	class UTextBlock* TimerText;

	UPROPERTY(VisibleAnywhere) 
	class APrototype2Gamestate* GameStateReference{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) 
	int32 CountdownTimer{}; // Countdown at beginning of game
};
