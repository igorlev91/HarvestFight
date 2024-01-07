

#include "Widget_PlayerName.h"

#include "FindInBlueprints.h"
#include "Components/TextBlock.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"

void UWidget_PlayerName::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerName->SetText(FText::FromString(""));

}

void UWidget_PlayerName::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerName->GetText().ToString() == FString{} )
	{
		UpdatePlayerName();
	}
}

void UWidget_PlayerName::UpdatePlayerName()
{
	if (auto* PlayerController = GetOwningPlayer())
	{
		if (auto PlayerState = PlayerController->GetPlayerState<ALobbyPlayerState>())
		{
			auto PlayerID = PlayerState->Player_ID;
		
			PlayerName->SetText(FText::FromString(FString::FromInt(PlayerID)));
		}
	}
}
