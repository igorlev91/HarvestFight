

#include "Widget_PlayerEmote.h"

void UWidget_PlayerEmote::SetPlayerRef(APlayerState* _Player)
{
	OwningPlayer = _Player;
}

APlayerState* UWidget_PlayerEmote::GetPlayerRef()
{
	return OwningPlayer;
}
