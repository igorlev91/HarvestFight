

#include "Widget_MapChoice.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

void UWidget_MapChoice::EnableMapChoice()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UWidget_MapChoice::SelectLevel(EFarm _Level)
{
	if (bHasMapBeenSelected == false)
	{
		bHasMapBeenSelected = true;

		if (ALobbyPlayerState* LobbyState = GetOwningPlayerState<ALobbyPlayerState>())
			LobbyState->VoteMap(_Level);
		else if (APrototype2PlayerState* GamePlayerState = GetOwningPlayerState<APrototype2PlayerState>())
			GamePlayerState->VoteMap(_Level);
	}
}


