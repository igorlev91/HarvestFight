

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

void UWidget_3DGrowUI::SetWeaponType(int32 _Type)
{
	/* HIDE FLOWERS */
	H_Flowers->SetVisibility(ESlateVisibility::Hidden);
	/* HIDE STARS */
	I_Stars->SetVisibility(ESlateVisibility::Hidden);
	
	/* UN-HIDE WEAPON */
	I_Weapon->SetVisibility(ESlateVisibility::Visible);

	if (WeaponTextures.Num() > _Type - 1)
	{
		I_Weapon->SetBrushFromTexture(WeaponTextures[_Type - 1]);
	}
}

void UWidget_3DGrowUI::SetStarCount(int32 _Count, bool _bGold)
{
	/* HIDE WEAPON */
	I_Weapon->SetVisibility(ESlateVisibility::Hidden);
	/* HIDE FLOWERS */
	H_Flowers->SetVisibility(ESlateVisibility::Hidden);
	
	/* UN-HIDE STARS */
	I_Stars->SetVisibility(ESlateVisibility::Visible);
	
	if (_Count <= 0)
	{
		I_Stars->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (_bGold)
	{
		if (GoldenStarTextures.Num() > _Count - 1)
			I_Stars->SetBrushFromTexture(GoldenStarTextures[_Count - 1]);
	}
	else
	{
		if (StarTextures.Num() > _Count - 1)
			I_Stars->SetBrushFromTexture(StarTextures[_Count - 1]);
	}
}

void UWidget_3DGrowUI::SetFlowerTypes(TArray<FFlowerData> _FlowerDatas)
{
	/* HIDE WEAPON */
	I_Weapon->SetVisibility(ESlateVisibility::Hidden);
	/* HIDE STARS */
	I_Stars->SetVisibility(ESlateVisibility::Hidden);
	
	/* UN-HIDE FLOWERS */
	H_Flowers->SetVisibility(ESlateVisibility::Visible);

	if (_FlowerDatas.Num() <= 0)
		return;

	if (FlowerWidgets.Num() > 0)
	{
		for(UWidget_Flower* Widget : FlowerWidgets)
		{
			Widget->RemoveFromParent();
			Widget->ConditionalBeginDestroy();
			Widget = nullptr;
		}
		FlowerWidgets.Empty();
	}

	for(FFlowerData FlowerData : _FlowerDatas)
	{
		UWidget_Flower* NewFlowerWidget = CreateWidget<UWidget_Flower>(GetOwningPlayer(), FlowerWidget);
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
	
	
	TimerMat->SetScalarParameterValue(FName("Percent"), ClampedProgress);

	if (ClampedProgress >= 1)
	{
		/* MAKE IT GREEN */
		//TimerMat->SetVectorParameterValue(FName("Color"), FLinearColor{0.0f, 1.0f, 0.0f});
		
		Timer->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Timer->SetVisibility(ESlateVisibility::Visible);
		TimerMat->SetVectorParameterValue(FName("Color"), _Gold ? FLinearColor{1.0f, 0.843f, 0.0f} : FLinearColor{1.0f, 1.0f, 1.0f});
	}

}
