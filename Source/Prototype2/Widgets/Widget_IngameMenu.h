

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_IngameMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_IngameMenu : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
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
};
