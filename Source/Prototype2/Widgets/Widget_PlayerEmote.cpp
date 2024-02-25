

#include "Widget_PlayerEmote.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

void UWidget_PlayerEmote::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWidget_PlayerEmote::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWidget_PlayerEmote::SetPlayerRef(APlayerState* _Player)
{
	OwningPlayer = _Player;
}

APlayerState* UWidget_PlayerEmote::GetPlayerRef()
{
	return OwningPlayer;
}

void UWidget_PlayerEmote::PlayEmote(EEmote _Emote)
{
	switch (_Emote)
	{
	case EEmote::Emote1:
		{
			EmoteImage->SetBrushFromTexture(Emote1);
			break;
		}
	case EEmote::Emote2:
		{
			EmoteImage->SetBrushFromTexture(Emote2);
			break;
		}
	case EEmote::Emote3:
		{
			if (GameStateReference->bTeams)
				EmoteImage->SetBrushFromTexture(Emote3);
			else
				EmoteImage->SetBrushFromTexture(Emote3);
			break;
		}
	case EEmote::Emote4:
		{
			if (GameStateReference->bTeams)
				EmoteImage->SetBrushFromTexture(Emote4);
			else
				EmoteImage->SetBrushFromTexture(Emote4);
			break;
		}
	case EEmote::Emote5:
		{
			if (GameStateReference->bTeams)
				EmoteImage->SetBrushFromTexture(Emote5);
			else
				EmoteImage->SetBrushFromTexture(Emote5);
			break;
		}
	case EEmote::Emote6:
		{
			if (GameStateReference->bTeams)
				EmoteImage->SetBrushFromTexture(Emote6);
			else
				EmoteImage->SetBrushFromTexture(Emote6);
			break;
		}
	case EEmote::Emote7:
		{
			if (GameStateReference->bTeams)
				EmoteImage->SetBrushFromTexture(Emote7);
			else
				EmoteImage->SetBrushFromTexture(Emote7);
			break;
		}
	case EEmote::Emote8:
		{
			if (GameStateReference->bTeams)
				EmoteImage->SetBrushFromTexture(Emote8);
			else
				EmoteImage->SetBrushFromTexture(Emote8);
			break;
		}
	default:
		{
			EmoteImage->SetBrushFromTexture(Emote1);
			break;
		}
	}
	
	OnPlayEmote();
}

void UWidget_PlayerEmote::OnPlayEmote_Implementation()
{
}
