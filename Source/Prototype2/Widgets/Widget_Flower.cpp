

#include "Widget_Flower.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWidget_Flower::SetFlowerType(int32 _StarValue)
{
	if (FlowerTextures.Num() > _StarValue - 1)
	{
		I_Flower->SetBrushFromTexture(FlowerTextures[_StarValue - 1]);
	}
}

void UWidget_Flower::SetFlowerCount(int32 _Amount)
{
	Count->SetText(FText::FromString(FString::FromInt(_Amount)));
}
