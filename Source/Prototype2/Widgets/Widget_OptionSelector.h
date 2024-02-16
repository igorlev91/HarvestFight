/* Bachelor of Software Engineering
 * Media Design School
 * Auckland
 * New Zealand
 * (c) Media Design School
 * File Name : Widget_OptionsSelector.h
 * Description : Widget for a option selection
 * Author/s : Stace Frear */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_OptionSelector.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_OptionSelector : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:

	/* Public Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) 
	class UTextBlock* OptionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget)) 
	UTextBlock* OptionValueText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ButtonLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ButtonRight;
};
