

#include "Widget_3DGrowUI.h"

#include "Widget_Flower.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"

void UWidget_3DGrowUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TimerMat = Timer->GetDynamicMaterial();
}

void UWidget_3DGrowUI::SetStarCount(int32 _Count)
{
	/* HIDE FLOWERS */
	H_Flowers->SetVisibility(ESlateVisibility::Hidden);
	/* UN-HIDE STARS */
	I_Stars->SetVisibility(ESlateVisibility::Visible);
	
	if (_Count <= 0)
	{
		I_Stars->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (StarTextures.Num() > _Count - 1)
		I_Stars->SetBrushFromTexture(StarTextures[_Count - 1]);
}

void UWidget_3DGrowUI::SetFlowerTypes(TArray<FFlowerData> _FlowerDatas)
{
	/* HIDE STARS */
	I_Stars->SetVisibility(ESlateVisibility::Hidden);
	/* UN-HIDE FLOWERS */
	H_Flowers->SetVisibility(ESlateVisibility::Visible);
	
	for(UWidget_Flower* Widget : FlowerWidgets)
	{
		Widget->RemoveFromParent();
		Widget->ConditionalBeginDestroy();
		Widget = nullptr;
	}
	FlowerWidgets.Empty();

	for(FFlowerData& FlowerData : _FlowerDatas)
	{
		UWidget_Flower* NewFlowerWidget = CreateWidget<UWidget_Flower>(this, FlowerWidget);
		NewFlowerWidget->SetFlowerType(FlowerData.StarValue);
		NewFlowerWidget->SetFlowerCount(FlowerData.Count);
		if (NewFlowerWidget)
		{
			UHorizontalBoxSlot* NewSlot = H_Flowers->AddChildToHorizontalBox(NewFlowerWidget);
			NewSlot->SetSize(ESlateSizeRule::Fill);
			H_Flowers->InvalidateLayoutAndVolatility();
		}
		FlowerWidgets.Add(NewFlowerWidget);
	}
}

void UWidget_3DGrowUI::SetGrowTimer(float _Progress, bool _Gold)
{
	if (TimerMat == nullptr)
		return;

	const float ClampedProgress = FMath::Clamp(_Progress, 0.0f, 1.0f);
	if (ClampedProgress >= 1)
		Timer->SetVisibility(ESlateVisibility::Hidden);
	else
		Timer->SetVisibility(ESlateVisibility::Visible);
	
	TimerMat->SetScalarParameterValue(FName("Percent"), ClampedProgress);
	TimerMat->SetVectorParameterValue(FName("Color"), _Gold ? FLinearColor{1.0f, 0.843f, 0.0f} : FLinearColor{1.0f, 1.0f, 1.0f});
}
