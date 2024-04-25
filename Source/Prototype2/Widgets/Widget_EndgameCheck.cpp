

#include "Widget_EndgameCheck.h"
#include "Kismet/GameplayStatics.h"

void UWidget_EndgameCheck::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HoverButton(bIsButtonHovered, ButtonToPulse, PulseTime, InDeltaTime, FVector2D(1.0f, 1.0f), ButtonScale);
}

void UWidget_EndgameCheck::ToggleMenu()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
