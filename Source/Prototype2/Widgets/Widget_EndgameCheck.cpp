

#include "Widget_EndgameCheck.h"
#include "Kismet/GameplayStatics.h"

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
