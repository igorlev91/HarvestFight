

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "UObject/Interface.h"
#include "WidgetUtility.generated.h"

class UButton;
UINTERFACE(NotBlueprintable)
class UWidgetUtilityInterface : public UInterface
{
	// This class does not need to be modified.
	GENERATED_BODY()
	// This class does not need to be modified.
};

class PROTOTYPE2_API IWidgetUtilityInterface
{
	GENERATED_BODY()
	/* Public Functions */
public:
	/**
	 * @brief Pulsing the button (if being hovered by keyboard or gamepad) 
	 * @param _bIsButtonHovered 
	 * @param _Button 
	 * @param _TimeBetweenPulse 
	 * @param _DeltaTime 
	 * @param _SmallScale 
	 * @param _LargeScale 
	 */
	UFUNCTION(BlueprintCallable)
	virtual void HoverButton(bool _bIsButtonHovered, UButton* _Button, float _TimeBetweenPulse, float _DeltaTime, FVector2D _SmallScale, FVector2D _LargeScale);

	/**
	 * @brief Returns whether a UI button is currently focussed by gamepad/keyboard
	 * @param _ButtonIndex 
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool IsButtonFocussed(int32 _ButtonIndex);
	
	
	/* Private Variables */
private:
	/* Timer used for button pulse */
	float PulseTimer{0.0f};
};
