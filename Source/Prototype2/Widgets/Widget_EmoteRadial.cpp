
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
