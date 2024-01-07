

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_EndgameCheck.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_EndgameCheck : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief Turns the endgame check menu widget on or off
	 */
	void ToggleMenu();
	
};
