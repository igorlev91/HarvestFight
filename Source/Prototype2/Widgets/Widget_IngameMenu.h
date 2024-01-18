
#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
#include "Widget_IngameMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_IngameMenu : public UUserWidget, public IWidgetUtilityInterface
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/**
	 * @brief Toggles menu widget on/off
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleMenu();
	
	/**
	 * @brief Turns off menu widget
	 */
	UFUNCTION(BlueprintCallable)
	void DisableMenu();

	/* Public Variables */
public:
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
