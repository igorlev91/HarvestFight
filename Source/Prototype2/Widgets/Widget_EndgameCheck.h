/* Bachelor of Software Engineering
 * Media Design School
 * Auckland
 * New Zealand
 * (c) Media Design School
 * File Name : Widget_EndgameCheck.h
 * Description : Toggles endgame widget on/off
 * Author/s : Stace Frear */

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
