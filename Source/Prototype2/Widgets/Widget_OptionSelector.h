

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
