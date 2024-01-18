

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
	FString PlayerStatePlayerName{};
	FLinearColor PlayerStateColour{};
	
	ALobbyPlayerState* LobbyPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
	if (LobbyPlayerState)
	{
		PlayerStatePlayerName = LobbyPlayerState->PlayerName;
		PlayerStateColour = FLinearColor(LobbyPlayerState->Details.CharacterColour);
	}

	APrototype2PlayerState* PrototypePlayerState = GetOwningPlayerState<APrototype2PlayerState>();
	if (PrototypePlayerState)
	{
		PlayerStatePlayerName = PrototypePlayerState->PlayerName;
		PlayerStateColour = FLinearColor(PrototypePlayerState->Details.CharacterColour);
	}
	
	PlayerName->SetText(FText::FromString(PlayerStatePlayerName));
	PlayerName->SetColorAndOpacity(PlayerStateColour);
}

void UWidget_PlayerName::SetPlayerRef(APlayerState* _Player)
{
	if (IsValid(_Player))
		OwningPlayer = _Player;
}

APlayerState* UWidget_PlayerName::GetPlayerRef()
{
	return OwningPlayer;
}

