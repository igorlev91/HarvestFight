

#include "Widget_LobbyCharacterSelection.h"

#include "Widget_LobbyPlayerHUDV2.h"
#include "Components/CircularThrobber.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Controllers/LobbyPlayerController.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/DataAssets/SkinData.h"

void UWidget_LobbyCharacterSelection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	/* Set gamestate reference */
	if (ALobbyGamestate* GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (IsValid(GameStateCast))
			GameStateReference = GameStateCast;
	}

	if (auto OwningPlayerController = GetOwningPlayer())
	{
		if (OwningPlayerController->HasAuthority())
		{
			ALobbyPlayerState* OwningPlayerstate = GetOwningPlayerState<ALobbyPlayerState>();
			if (IsValid(OwningPlayerstate))
				IdealDetails = OwningPlayerstate->Details;
			
			bPlayerstateUpdated = true;
			UpdateCharacterImage();
		}
	}
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	ALobbyPlayerState* OwningPlayerstate = GetOwningPlayerState<ALobbyPlayerState>();
	if (IsValid(OwningPlayerstate) == false)
		return;
	
	EColours OwningPlayerColor = OwningPlayerstate->Details.Colour;
	
	if (SkinColourData && SkinColourData->PureColours.Num() > (uint8)OwningPlayerColor)
	{
		PlayerColourImage->SetColorAndOpacity(SkinColourData->PureColours[(uint8)OwningPlayerColor]);
	}
	
	switch(IdealDetails.Character)
	{
	case ECharacters::COW:
		{
			PlayerImage->SetBrushFromTexture(OwningPlayerstate->CowTextures[(uint8)OwningPlayerColor]);
			break;
		}
	case ECharacters::CHICKEN:
		{
			PlayerImage->SetBrushFromTexture(OwningPlayerstate->ChickenTextures[(uint8)OwningPlayerColor]);
			break;
		}
	case ECharacters::DUCK:
		{
			PlayerImage->SetBrushFromTexture(OwningPlayerstate->DuckTextures[(uint8)OwningPlayerColor]);
			break;
		}
	case ECharacters::PIG:
		{
			PlayerImage->SetBrushFromTexture(OwningPlayerstate->PigTextures[(uint8)OwningPlayerColor]);
			break;
		}
	case ECharacters::BEE:
		{
			PlayerImage->SetBrushFromTexture(OwningPlayerstate->BeeTextures[(uint8)OwningPlayerColor]);
			break;
		}
	default:
		{
			UE_LOG(LogTemp, Warning, TEXT("Error: Widget_LobbyCharacterSelection: Unable to determine character type"));
			break;
		}
	}
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColour(bool _bIsTowardsRight)
{
	if (bPlayerstateUpdated == false)
		return;

	TArray<EColours> AvailableColors = GetAvailableColours();
	int16 NewColor = AvailableColors.Find(IdealDetails.Colour);
	if (_bIsTowardsRight)
	{
		NewColor++;
	}
	else
	{
		NewColor--;
	}
	if (NewColor >= AvailableColors.Num())
	{
		NewColor = 0;
	}
	else if (NewColor < 0)
	{
		NewColor = AvailableColors.Num() - 1;
	}
	IdealDetails.Colour = AvailableColors[NewColor];
	SetCharacterColourFromSelection();
	
	bPlayerstateUpdated = false;
	GetOwningPlayerState<ALobbyPlayerState>()->UpdateCharacterMaterial(IdealDetails);
}

bool UWidget_LobbyCharacterSelection::ChangeTeamsCharacterColour(bool _bIsTowardsRight)
{
	if (bPlayerstateUpdated == false)
		return false;

	if (CanChangeTeams() == false)
		return false;

	auto OwningPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
	if (IsValid(OwningPlayerState) == false)
		return false;

	auto GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState) == false)
		return false;
	
	if (OwningPlayerState->Details.Colour == GameState->TeamsDetails.TeamOneColour)
		IdealDetails.Colour = GameState->TeamsDetails.TeamTwoColour;
	else
		IdealDetails.Colour = GameState->TeamsDetails.TeamOneColour;
	
	SetCharacterColourFromSelection();
	
	bPlayerstateUpdated = false;
	GetOwningPlayerState<ALobbyPlayerState>()->UpdateCharacterMaterial(IdealDetails);

	return true;
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _bIsTowardsRight)
{
	if (bPlayerstateUpdated == false)
		return;
	
	int16 newCharacter = (uint8)IdealDetails.Character;
	if (_bIsTowardsRight)
	{
		newCharacter++;
	}
	else
	{
		newCharacter--;
	}
	if (newCharacter >= NumberOfCharacters)
	{
		newCharacter = 0;
	}
	else if (newCharacter < 0)
	{
		newCharacter = NumberOfCharacters - 1;
	}
	IdealDetails.Character = (ECharacters)newCharacter;
	
	SetCharacterModelFromSelection();
	SetCharacterColourFromSelection();
	
	bPlayerstateUpdated = false;
	GetOwningPlayerState<ALobbyPlayerState>()->UpdateCharacterMaterial(IdealDetails);
}

void UWidget_LobbyCharacterSelection::SetCharacterColourFromSelection()
{
	if (!SkinColourData)
		return;

	IdealDetails.PureToneColour = SkinColourData->PureColours[(int16) IdealDetails.Colour];

	switch(IdealDetails.Colour)
	{
	case EColours::RED:
		{
			if (SkinColourData->Reds.Num() > (int16)IdealDetails.Character)
				IdealDetails.CharacterColour = SkinColourData->Reds[(int16)IdealDetails.Character];

			if (SkinColourData->SubReds.Num() > (int16)IdealDetails.Character)
				IdealDetails.CharacterSubColour = SkinColourData->SubReds[(int16)IdealDetails.Character];
			
			break;
		}
	case EColours::BLUE:
		{
			IdealDetails.CharacterColour = SkinColourData->Blue;
			IdealDetails.CharacterSubColour = SkinColourData->SubBlue;
			break;
		}
	case EColours::GREEN:
		{
			IdealDetails.CharacterColour = SkinColourData->Green;
			IdealDetails.CharacterSubColour = SkinColourData->SubGreen;
			break;
		}
	case EColours::YELLOW:
		{
			IdealDetails.CharacterColour = SkinColourData->Yellow;
			IdealDetails.CharacterSubColour = SkinColourData->SubYellow;
			break;
		}
	case EColours::PURPLE:
		{
			IdealDetails.CharacterColour = SkinColourData->Purple;
			IdealDetails.CharacterSubColour = SkinColourData->SubPurple;
			break;
		}
	case EColours::ORANGE:
		{
			IdealDetails.CharacterColour = SkinColourData->Orange;
			IdealDetails.CharacterSubColour = SkinColourData->SubOrange;
			break;
		}
	case EColours::BLACK:
		{
			IdealDetails.CharacterColour = SkinColourData->Black;
			IdealDetails.CharacterSubColour = SkinColourData->SubBlack;
			break;
		}
	default:
		{
			if (SkinColourData->Whites.Num() > (int16)IdealDetails.Character)
				IdealDetails.CharacterColour = SkinColourData->Whites[(int16)IdealDetails.Character];

			if (SkinColourData->SubWhites.Num() > (int16)IdealDetails.Character)
				IdealDetails.CharacterSubColour = SkinColourData->SubWhites[(int16)IdealDetails.Character];
			break;
		}
	}
}

void UWidget_LobbyCharacterSelection::SetCharacterModelFromSelection()
{
	if (SkinData->Models.Num() > (int16)IdealDetails.Character)
		IdealDetails.AnimationData = SkinData->Models[(int16)IdealDetails.Character];
}

void UWidget_LobbyCharacterSelection::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateButtonVisibility();
	UpdateWidgetSwitchers();

	if (bPlayerstateUpdated)
		UpdateCharacterImage();
	
	auto OwningPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
	if (IsValid(OwningPlayerState) == false)
		return;

	if (OwningPlayerState->Details.Character != IdealDetails.Character
		|| OwningPlayerState->Details.Colour != IdealDetails.Colour)
	{
		IdealDetails = OwningPlayerState->Details;
	}
}

void UWidget_LobbyCharacterSelection::UpdateButtonVisibility()
{
	if (bLocalReady)
	{
		Button_LeftColour->SetVisibility(ESlateVisibility::Hidden);
		Button_RightColour->SetVisibility(ESlateVisibility::Hidden);
		Button_LeftCharacter->SetVisibility(ESlateVisibility::Hidden);
		Button_RightCharacter->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	
	Button_LeftCharacter->SetVisibility(ESlateVisibility::Visible);
	Button_RightCharacter->SetVisibility(ESlateVisibility::Visible);

	if (bTeams)
	{
		auto OwningPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
		if (IsValid(OwningPlayerState) == false)
			return;

		auto GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
		if (IsValid(GameState) == false)
			return;

		bool bOnTeamOne = OwningPlayerState->Details.Colour == GameState->TeamsDetails.TeamOneColour;
		if /* TEAM ONE */ (bOnTeamOne)
		{
			Button_LeftColour->SetVisibility(ESlateVisibility::Hidden);
			Button_RightColour->SetVisibility(ESlateVisibility::Visible);
		}
		else /* TEAM TWO */
		{
			Button_LeftColour->SetVisibility(ESlateVisibility::Visible);
			Button_RightColour->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		Button_LeftColour->SetVisibility(ESlateVisibility::Visible);
		Button_RightColour->SetVisibility(ESlateVisibility::Visible);
	}
}

bool UWidget_LobbyCharacterSelection::HasSamePlayerColour()
{
	ALobbyPlayerState* OwningPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
	if (IsValid(OwningPlayerState) == false)
		return true;
	
	for (int16 i = 0; i < GameStateReference->PlayerArray.Num(); i++)
	{
		if (ALobbyPlayerState* SomePlayer = Cast<ALobbyPlayerState>(GameStateReference->PlayerArray[i]))
		{
			if (OwningPlayerState != SomePlayer)
			{
				if (SomePlayer->Details.Colour == IdealDetails.Colour)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

void UWidget_LobbyCharacterSelection::SetOwningController(ALobbyPlayerController* _Owner)
{
	OwningController = _Owner;
}

void UWidget_LobbyCharacterSelection::SetPlayerID(int32 _PlayerID)
{
	PlayerID = _PlayerID;
}

bool UWidget_LobbyCharacterSelection::CanChangeTeams()
{
	auto OwningPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
	if (IsValid(OwningPlayerState) == false)
		return false;

	auto GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState) == false)
		return false;

	bool bOnTeamOne = OwningPlayerState->Details.Colour == GameState->TeamsDetails.TeamOneColour;
	int16 TeamOneCount = GameState->TeamsDetails.Server_TeamOne.Num();
	int16 TeamTwoCount = GameState->TeamsDetails.Server_TeamTwo.Num();

	bool bCanChangeTeams = true;
	if /* ONE -> TWO */ (bOnTeamOne)
	{
		if (TeamTwoCount > TeamOneCount)
			bCanChangeTeams = false;
	}
	else /* TWO -> ONE */
	{
		if (TeamOneCount > TeamTwoCount)
			bCanChangeTeams = false;
	}
	return bCanChangeTeams;
}

TArray<EColours> UWidget_LobbyCharacterSelection::GetAvailableColours()
{
	TArray<EColours> AvailableColors{
		EColours::RED,
		EColours::BLUE,
		EColours::GREEN,
		EColours::YELLOW,
		EColours::PURPLE,
		EColours::ORANGE,
		EColours::BLACK,
		EColours::WHITE
	};

	ALobbyPlayerState* OwningPlayerState = GetOwningPlayerState<ALobbyPlayerState>();
	if (IsValid(OwningPlayerState) == false)
		return {OwningPlayerState->Details.Colour};
	
	for (auto Player : GameStateReference->Server_Players)
	{
		if (Player.Get() != OwningPlayerState)
		{
			AvailableColors.Remove(Player->Details.Colour);
		}
	}

	return AvailableColors;
}

void UWidget_LobbyCharacterSelection::UpdateWidgetSwitchers()
{
	if /* Switch With Button */ (bPlayerstateUpdated)
	{
		Switch_ColorLeft->SetActiveWidgetIndex(0);
		Switch_ColorRight->SetActiveWidgetIndex(0);
		Switch_CharacterLeft->SetActiveWidgetIndex(0);
		Switch_CharacterRight->SetActiveWidgetIndex(0);
	}
	else /* Switch With Loading */
	{
		if (Button_LeftColour->IsVisible())
			Switch_ColorLeft->SetActiveWidgetIndex(1);
		if (Button_RightColour->IsVisible())
			Switch_ColorRight->SetActiveWidgetIndex(1);
		if (Button_LeftCharacter->IsVisible())
			Switch_CharacterLeft->SetActiveWidgetIndex(1);
		if (Button_RightCharacter->IsVisible())
			Switch_CharacterRight->SetActiveWidgetIndex(1);
	}
}
