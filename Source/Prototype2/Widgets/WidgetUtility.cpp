
#include "WidgetUtility.h"
#include "Components/Button.h"

void IWidgetUtilityInterface::HoverButton(bool _bIsButtonHovered, UButton* _Button, float _TimeBetweenPulse, float _DeltaTime, FVector2D _SmallScale, FVector2D _LargeScale)
{
	if (!_bIsButtonHovered)
		return;
	
	PulseTimer += _DeltaTime;
	if (PulseTimer >= _TimeBetweenPulse)
	{
		if (_Button->GetRenderTransform().Scale.X == _SmallScale.X)
		{
			_Button->SetRenderScale(_LargeScale);
		}
		else
		{
			_Button->SetRenderScale(_SmallScale);
		}
		
		PulseTimer = 0;
	}
}

bool IWidgetUtilityInterface::IsButtonFocussed(int32 _ButtonIndex)
{
	if (_ButtonIndex >= 0)
		return true;
	
	return false;
}
