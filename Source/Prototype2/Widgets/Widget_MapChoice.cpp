

#include "Widget_MapChoice.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"

void UWidget_MapChoice::EnableMapChoice()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UWidget_MapChoice::SelectLevel(EFarm _Level)
{
	if (bHasMapBeenSelected == false)
	{
		bHasMapBeenSelected = true;

		if (auto* PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
		{
			auto PlayerID = PlayerController->GetPlayerState<ALobbyPlayerState>()->Player_ID;
			PlayerController->VoteMap(PlayerID, _Level);
		}
	}
}


