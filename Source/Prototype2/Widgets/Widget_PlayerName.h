

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerName.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_PlayerName : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions*/
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdatePlayerName();
	
	/* Public Variables*/
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* PlayerName; // Name for player

	/* Private Varables */
private:
	FLinearColor Player1Colour{199.0f/256.0f, 50.0f/256.0f, 35.0f/256.0f, 1.0f}; // Red
	FLinearColor Player2Colour{14.0f/256.0f, 90.0f/256.0f, 209.0f/256.0f, 1.0f}; // Blue
	FLinearColor Player3Colour{55.0f/256.0f, 181.0f/256.0f, 21.0f/256.0f, 1.0f}; // Green
	FLinearColor Player4Colour{255.0f/256.0f, 193.0f/256.0f, 9.0f/256.0f, 1.0f}; // Yellow
	
};
