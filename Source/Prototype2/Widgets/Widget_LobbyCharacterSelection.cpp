

#include "Widget_LobbyCharacterSelection.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/DataAssets/SkinData.h"

void UWidget_LobbyCharacterSelection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	/* Set gamestate reference */
	if (auto GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameStateCast;
	}
	
	OwningController = Cast<APrototype2PlayerController>(GetOwningPlayer());
	if (OwningController)
	{
		if (ALobbyPlayerState* PlayerState = GetOwningPlayerState<ALobbyPlayerState>())
		{
			if (!HasBeenAssignedTeamColour)
			{
				IdealDetails.Colour = PlayerState->Details.Colour;
			
				SetCharacterModelFromSelection(NumberOfCharacters);
				SetCharacterColourFromSelection((int32)EColours::MAXCOLOURS);
				OwningController->SyncPlayerMaterial(PlayerState->Player_ID, IdealDetails);
				UpdateCharacterImage(OwningController);
			}
		}
	}
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage(class APrototype2PlayerController* _Owner)
{
	if (SkinColourData && SkinColourData->PureColours.Num() > (uint8)IdealDetails.Colour)
	{
		PlayerColourImage->SetColorAndOpacity(SkinColourData->PureColours[(uint8)IdealDetails.Colour]);
	}
	
	if (!_Owner)
	{
		return;
	}

	ALobbyPlayerState* Player = _Owner->GetPlayerState<ALobbyPlayerState>();
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

		if (!OwningController)
		return;
	
	OwningController->SyncPlayerMaterial(PlayerID, IdealDetails);
	UpdateCharacterImage(OwningController);	
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _bIsTowardsRight)
{
	if (!OwningController)
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
		
	SetCharacterModelFromSelection(NumberOfCharacters);

	OwningController->SyncPlayerMaterial(PlayerID, IdealDetails);
	UpdateCharacterImage(OwningController);	
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
	
	if (!OwningController)
		return;

	if (bTeams)
	{
		if (ALobbyPlayerState* PlayerState = OwningController->GetPlayerState<ALobbyPlayerState>())
		{
			if (!HasBeenAssignedTeamColour)
			{
				IdealDetails.Colour = PlayerState->Details.Colour;
			
				SetCharacterModelFromSelection(NumberOfCharacters);
				SetCharacterColourFromSelection((int32)EColours::MAXCOLOURS);
				OwningController->SyncPlayerMaterial(PlayerState->Player_ID, IdealDetails);
				UpdateCharacterImage(OwningController);

				if (PlayerState->Player_ID != -1)
					HasBeenAssignedTeamColour = true;
			}
		}
		return;
	}

	if (OwningController->HasAuthority())
		return;

	if (!HasSamePlayerColour())
		return;

	for (int16 j = 0; j < (int32)EColours::MAXCOLOURS; j++)
	{
		if (!HasSamePlayerColour())
		{
			SetCharacterModelFromSelection(NumberOfCharacters);
			SetCharacterColourFromSelection((int32)EColours::MAXCOLOURS);
			OwningController->SyncPlayerMaterial(PlayerID, IdealDetails);
			UpdateCharacterImage(OwningController);
			return;
		}
		else
		{
			ChangeCharacterColour(true);
		}
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

void UWidget_LobbyCharacterSelection::SetOwningController(int32 _PlayerID, APrototype2PlayerController* _Owner)
{
	OwningController = _Owner;
	PlayerID = _PlayerID;
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
