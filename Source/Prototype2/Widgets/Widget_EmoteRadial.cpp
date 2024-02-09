

#include "Widget_EmoteRadial.h"

#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"


void UWidget_EmoteRadial::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}
}

void UWidget_EmoteRadial::SetEmote(EEmote _Emote)
{
    if (auto PlayerState = Cast<APrototype2PlayerState>(GetOwningPlayerState()))
    {
    	auto PlayerID = PlayerState->Player_ID;
		   
    	if (GameStateReference->Server_Players.Num() >= PlayerID)
    	{
    		if (auto PlayerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
    		{
			   PlayerController->SetPlayerEmote(PlayerID, _Emote);
    		}
    	}
    }
}
