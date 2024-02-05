
#include "Widget_SellCropUI.h"

#include "Components/TextBlock.h"

void UWidget_SellCropUI::SetCropValue(int32 _Value)
{
	FString FirstString = "$";
	FString SecondString = FString::FromInt(_Value);

	FString CombinedString = FString::Printf(TEXT("%s %s"), *FirstString, *SecondString);

	if (SellText)
		SellText->SetText(FText::FromString(CombinedString));
}
