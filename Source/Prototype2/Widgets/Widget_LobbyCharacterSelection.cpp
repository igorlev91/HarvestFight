

#include "Widget_LobbyCharacterSelection.h"

#include "Components/Image.h"
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

	if (ALobbyPlayerState* PlayerStateCast = GetOwningPlayerState<ALobbyPlayerState>())
	{
		if (IsValid(PlayerStateCast))
		{
			IdealDetails = PlayerStateCast->Details;
			UpdateCharacterImage();
		}
	}
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	if (SkinColourData && SkinColourData->PureColours.Num() > (uint8)IdealDetails.Colour)
	{
		PlayerColourImage->SetColorAndOpacity(SkinColourData->PureColours[(uint8)IdealDetails.Colour]);
	}

	ALobbyPlayerState* Player = GetOwningPlayerState<ALobbyPlayerState>();
	if (!Player)
		return;
	
	switch(IdealDetails.Character)
	{
	case ECharacters::COW:
		{
			PlayerImage->SetBrushFromTexture(Player->CowTextures[(uint8)IdealDetails.Colour]);
			break;
		}
	case ECharacters::CHICKEN:
		{
			PlayerImage->SetBrushFromTexture(Player->ChickenTextures[(uint8)IdealDetails.Colour]);
			break;
		}
	case ECharacters::DUCK:
		{
			PlayerImage->SetBrushFromTexture(Player->DuckTextures[(uint8)IdealDetails.Colour]);
			break;
		}
	case ECharacters::PIG:
		{
			PlayerImage->SetBrushFromTexture(Player->PigTextures[(uint8)IdealDetails.Colour]);
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
	for (int16 i = 0; i < (int32)EColours::MAXCOLOURS; i++)
	{
		int16 newColour = (uint8)IdealDetails.Colour;
		if (_bIsTowardsRight)
		{
			newColour++;
		}
		else
		{
			newColour--;
		}
		if (newColour >= (int32)EColours::MAXCOLOURS)
		{
			newColour = 0;
		}
		else if (newColour < 0)
		{
			newColour = (int32)EColours::MAXCOLOURS - 1;
		}
		IdealDetails.Colour = (EColours)newColour;
		SetCharacterColourFromSelection((int32)EColours::MAXCOLOURS);

		if (!HasSamePlayerColour())
			break;
	}
	
	GetOwningPlayerState<ALobbyPlayerState>()->UpdateCharacterMaterial(IdealDetails);
	UpdateCharacterImage();	
}

bool UWidget_LobbyCharacterSelection::ChangeTeamsCharacterColour(bool _bIsTowardsRight)
{
	ALobbyGamestate* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(GameState))
		return false;

	if (!CanChangeTeams(_bIsTowardsRight))
		return false;

	if (_bIsTowardsRight)
	{
		Button_LeftColour->SetVisibility(ESlateVisibility::Visible);
		Button_RightColour->SetVisibility(ESlateVisibility::Hidden);
		Button_LeftColour->InvalidateLayoutAndVolatility();
		Button_RightColour->InvalidateLayoutAndVolatility();

		PreviousColorButtonVisibilities[0] = Button_LeftColour->GetVisibility();
		PreviousColorButtonVisibilities[1] = Button_RightColour->GetVisibility();
	}
	else
	{
		Button_LeftColour->SetVisibility(ESlateVisibility::Hidden);
		Button_RightColour->SetVisibility(ESlateVisibility::Visible);
		Button_LeftColour->InvalidateLayoutAndVolatility();
		Button_RightColour->InvalidateLayoutAndVolatility();

		PreviousColorButtonVisibilities[0] = Button_LeftColour->GetVisibility();
		PreviousColorButtonVisibilities[1] = Button_RightColour->GetVisibility();
	}
	
	if (IdealDetails.Colour == GameState->TeamsDetails.TeamOneColour)
		IdealDetails.Colour = GameState->TeamsDetails.TeamTwoColour;
	else
		IdealDetails.Colour = GameState->TeamsDetails.TeamOneColour;
	
	SetCharacterColourFromSelection((int32)EColours::MAXCOLOURS);
	GetOwningPlayerState<ALobbyPlayerState>()->UpdateCharacterMaterial(IdealDetails);
	UpdateCharacterImage();

	return true;
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _bIsTowardsRight)
{
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
		
	SetCharacterModelFromSelection(NumberOfCharacters);
	SetCharacterColourFromSelection((int32)EColours::MAXCOLOURS);
	GetOwningPlayerState<ALobbyPlayerState>()->UpdateCharacterMaterial(IdealDetails);
	UpdateCharacterImage();	
}

void UWidget_LobbyCharacterSelection::SetCharacterColourFromSelection(int32 _NumberOfColors)
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

void UWidget_LobbyCharacterSelection::SetCharacterModelFromSelection(int32 _NumberOfCharacters)
{
	if (SkinData->Models.Num() > (int16)IdealDetails.Character)
		IdealDetails.AnimationData = SkinData->Models[(int16)IdealDetails.Character];
}

void UWidget_LobbyCharacterSelection::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWidget_LobbyCharacterSelection::SetSkinSelectionVisibility(ESlateVisibility _Visiblity)
{
	if (bTeams)
	{
		Button_LeftCharacter->SetVisibility(_Visiblity);
		Button_RightCharacter->SetVisibility(_Visiblity);

		if (_Visiblity == ESlateVisibility::Hidden)
		{
			if (PreviousColorButtonVisibilities.Num() <= 0)
			{
				PreviousColorButtonVisibilities.Add(Button_LeftColour->GetVisibility());
				PreviousColorButtonVisibilities.Add(Button_RightColour->GetVisibility());
			}
			else
			{
				PreviousColorButtonVisibilities[0] = Button_LeftColour->GetVisibility();
				PreviousColorButtonVisibilities[1] = Button_RightColour->GetVisibility();
			}
			
			Button_LeftColour->SetVisibility(_Visiblity);
			Button_RightColour->SetVisibility(_Visiblity);
		}
		else
		{
			Button_LeftColour->SetVisibility(PreviousColorButtonVisibilities[0]);
			Button_RightColour->SetVisibility(PreviousColorButtonVisibilities[1]);
		}
	}
	else
	{
		Button_LeftColour->SetVisibility(_Visiblity);
		Button_RightColour->SetVisibility(_Visiblity);
		Button_LeftCharacter->SetVisibility(_Visiblity);
		Button_RightCharacter->SetVisibility(_Visiblity);
	}
}

bool UWidget_LobbyCharacterSelection::HasSamePlayerColour()
{
	for (int16 i = 0; i < GameStateReference->PlayerArray.Num(); i++)
	{
		if (ALobbyPlayerState* SomePlayer = Cast<ALobbyPlayerState>(GameStateReference->PlayerArray[i]))
		{
			if (OwningController)
			{
				if (OwningController->GetPlayerState<ALobbyPlayerState>() != SomePlayer)
				{
					if (SomePlayer->Details.Colour == IdealDetails.Colour)
					{
						return true;
					}
				}
			}
			else
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
	PlayerID = 0;
}

void UWidget_LobbyCharacterSelection::SetPlayerID(int32 _PlayerID)
{
	PlayerID = _PlayerID;
}

int32 UWidget_LobbyCharacterSelection::GetNumberOfRedPlayers()
{
	if (!OwningController)
		return 0;
	
	int32 Count{};
	if (IdealDetails.Colour == EColours::RED)
		Count = 1;
	for (int16 i = 0; i < GameStateReference->PlayerArray.Num(); i++)
	{
		if (ALobbyPlayerState* SomePlayer = Cast<ALobbyPlayerState>(GameStateReference->PlayerArray[i]))
		{
			if (OwningController->GetPlayerState<ALobbyPlayerState>() != SomePlayer)
			{
				if (SomePlayer->Details.Colour == EColours::RED)
					Count++;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Number Of Red Players: %s"), *FString::FromInt(Count));
	return Count;
}

int32 UWidget_LobbyCharacterSelection::GetNumberOfBluePlayers()
{
	if (!OwningController)
		return 0;
	
	int32 Count{};
	if (IdealDetails.Colour == EColours::BLUE)
		Count = 1;
	for (int16 i = 0; i < GameStateReference->PlayerArray.Num(); i++)
	{
		if (ALobbyPlayerState* SomePlayer = Cast<ALobbyPlayerState>(GameStateReference->PlayerArray[i]))
		{
			if (OwningController->GetPlayerState<ALobbyPlayerState>() != SomePlayer)
			{
				if (SomePlayer->Details.Colour == EColours::BLUE)
					Count++;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Number Of Blue Players: %s"), *FString::FromInt(Count));
	return Count;
}

void UWidget_LobbyCharacterSelection::OnRep_TeamsDetails(bool _TeamA)
{
	if (bLocalReady)
		return;
	
	if (_TeamA)
	{
		Button_LeftColour->SetVisibility(ESlateVisibility::Hidden);
		Button_RightColour->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Button_LeftColour->SetVisibility(ESlateVisibility::Visible);
		Button_RightColour->SetVisibility(ESlateVisibility::Hidden);
	}

	if (PreviousColorButtonVisibilities.Num() <= 0)
	{
		PreviousColorButtonVisibilities.Add(Button_LeftColour->GetVisibility());
		PreviousColorButtonVisibilities.Add(Button_RightColour->GetVisibility());
	}
	else
	{
		PreviousColorButtonVisibilities[0] = Button_LeftColour->GetVisibility();
		PreviousColorButtonVisibilities[1] = Button_RightColour->GetVisibility();
	}
}

bool UWidget_LobbyCharacterSelection::CanChangeTeams(bool _ChangeRight)
{
	bool bCanChange = true;

	if (_ChangeRight)
	{
		// True for empty lobby
		if (GameStateReference->TeamsDetails.Server_TeamTwo.Num() <= 0)
			return true;
		
		if (GameStateReference->TeamsDetails.Server_TeamTwo.Num()
			>
			GameStateReference->TeamsDetails.Server_TeamOne.Num())
				bCanChange = false;
		else if ((GameStateReference->TeamsDetails.Server_TeamOne.Num() - 1) < GameStateReference->TeamsDetails.Server_TeamTwo.Num())
				bCanChange = false;
		else if (GameStateReference->TeamsDetails.Server_TeamTwo.Num() + 1 > 3)
			bCanChange = false;
	}
	else
	{
		// True for empty lobby
		if (GameStateReference->TeamsDetails.Server_TeamOne.Num() <= 0)
			return true;

		if (GameStateReference->TeamsDetails.Server_TeamOne.Num()
			>
			GameStateReference->TeamsDetails.Server_TeamTwo.Num())
				bCanChange = false;
		else if ((GameStateReference->TeamsDetails.Server_TeamTwo.Num() - 1) < GameStateReference->TeamsDetails.Server_TeamOne.Num())
				bCanChange = false;
		else if (GameStateReference->TeamsDetails.Server_TeamOne.Num() + 1 > 3)
			bCanChange = false;
	}

	return bCanChange;
}
