

#include "Widget_PlayerEmote.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

void UWidget_PlayerEmote::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/* Get gamestate and set to reference variable */
	if (auto* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}
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
	if (!GameStateReference)
		return;
	
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
			//if (GameStateReference->bTeams)
			//	EmoteImage->SetBrushFromTexture(Emote9);
			//else
				EmoteImage->SetBrushFromTexture(Emote3);
			break;
		}
	case EEmote::Emote4:
		{
			//if (GameStateReference->bTeams)
			//	EmoteImage->SetBrushFromTexture(Emote10);
			//else
				EmoteImage->SetBrushFromTexture(Emote4);
			break;
		}
	case EEmote::Emote5:
		{
			//if (GameStateReference->bTeams)
			//	EmoteImage->SetBrushFromTexture(Emote11);
			//else
				EmoteImage->SetBrushFromTexture(Emote5);
			break;
		}
	case EEmote::Emote6:
		{
			//if (GameStateReference->bTeams)
			//	EmoteImage->SetBrushFromTexture(Emote12);
			//else
				EmoteImage->SetBrushFromTexture(Emote6);
			break;
		}
	case EEmote::Emote7:
		{
			//if (GameStateReference->bTeams)
			//	EmoteImage->SetBrushFromTexture(Emote13);
			//else
				EmoteImage->SetBrushFromTexture(Emote7);
			break;
		}
	case EEmote::Emote8:
		{
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
