

#include "Widget_PlayerName.h"

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
			auto PlayerStatePlayerID = PlayerState->Player_ID;
			auto PlayerStatePlayerName = PlayerState->PlayerName;
			
			PlayerName->SetText(FText::FromString(PlayerStatePlayerName));
			
			/* Set colour of player name */
			switch (PlayerStatePlayerID)
			{
			case 0:
				{
					PlayerName->SetColorAndOpacity(Player1Colour); // Player 1
					break;
				}
			case 1:
				{
					PlayerName->SetColorAndOpacity(Player2Colour); // Player 2
					break;
				}
			case 2:
				{
					PlayerName->SetColorAndOpacity(Player3Colour); // Player 3
					break;
				}
			case 3:
				{
					PlayerName->SetColorAndOpacity(Player4Colour); // Player 4
					break;
				}
			default:
				{
					PlayerName->SetColorAndOpacity(Player1Colour);
					break;
				}
			}
			
			
		}
	}
}
