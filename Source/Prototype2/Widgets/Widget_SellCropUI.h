

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_SellCropUI.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_SellCropUI : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	void SetCropValue(int32 _Value);

	/* Public Variables */
public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* SellText; // Amount player has earned from sale


};
