

#include "Widget_PlayerName.h"

#include "Components/TextBlock.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

void UWidget_PlayerName::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerName->SetText(FText::FromString(""));
}

void UWidget_PlayerName::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	UpdatePlayerName();
}

void UWidget_PlayerName::UpdatePlayerName()
{
	if (!OwningPlayer)
		return;
	
	if (APrototype2PlayerState* PrototypePlayerState = Cast<APrototype2PlayerState>(OwningPlayer))
	{
		PlayerName->SetText(FText::FromString(PrototypePlayerState->PlayerName));
		PlayerName->SetColorAndOpacity((FLinearColor)PrototypePlayerState->Details.PureToneColour);
	}
	else if (ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(OwningPlayer))
	{
		PlayerName->SetText(FText::FromString(LobbyPlayerState->PlayerName));
		PlayerName->SetColorAndOpacity((FLinearColor)LobbyPlayerState->Details.PureToneColour);
	}
	//
}

void UWidget_PlayerName::SetPlayerRef(APlayerState* _Player)
{
	OwningPlayer = _Player;
}

APlayerState* UWidget_PlayerName::GetPlayerRef()
{
	return OwningPlayer;
}

