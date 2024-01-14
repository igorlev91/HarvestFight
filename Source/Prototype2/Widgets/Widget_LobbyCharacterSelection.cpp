// Fill out your copyright notice in the Description page of Project Settings.


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

void UWidget_LobbyCharacterSelection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayerImage->SetBrushFromTexture(Texture_CowRed);

	if (OwningController)
	{
		OwningController->SyncPlayerMaterial(IdealDetails);
	}
	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	switch(IdealDetails.Character)
	{
	case ECharacters::COW:
		{
			if (CowTextures.Num() > 0)
			{
				PlayerImage->SetBrushFromTexture(CowTextures[0]);
			}
			break;
		}
	case ECharacters::CHICKEN:
		{
			if (ChickenTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(ChickenTextures[0]);
			break;
		}
	case ECharacters::DUCK:
		{
			if (DuckTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(DuckTextures[0]);
			break;
		}
	case ECharacters::PIG:
		{
			if (PigTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(PigTextures[0]);
			break;
		}
	default:
		{
			if (CowTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(CowTextures[0]);
			break;
		}
	}

	if (SkinColourData && SkinColourData->PureColours.Num() > (int16)CurrentColourSelection)
	{
		PlayerColourImage->SetColorAndOpacity(SkinColourData->PureColours[(int16)CurrentColourSelection]);
	}
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColour(bool _bIsTowardsRight)
{
	int16 newColour = (int16)CurrentColourSelection;
	if (_bIsTowardsRight)
	{
		newColour++;
	}
	else
	{
		newColour--;
	}
	if (newColour >= NumberOfColours)
	{
		newColour = 0;
	}
	else if (newColour < 0)
	{
		newColour = NumberOfColours - 1;
	}
	CurrentColourSelection = (EColours)newColour;
	
	SetCharacterColourFromSelection(NumberOfColours);

	if (OwningController)
	{
		OwningController->SyncPlayerMaterial(IdealDetails);
	}
	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _bIsTowardsRight)
{
	int16 NewCharacter = (int16)IdealDetails.Character;
	if (_bIsTowardsRight)
	{
		NewCharacter ++;
	}
	else
	{
		NewCharacter --;
	}
	
	if (NewCharacter > 3)
	{
		NewCharacter = 0;
	}
	else if (NewCharacter < 0)
	{
		NewCharacter = 3;
	}
	IdealDetails.Character = ECharacters(NewCharacter);
	
	CheckForTakenCharacter(_bIsTowardsRight);

	if (OwningController)
	{
		OwningController->SyncPlayerMaterial(IdealDetails);
	}
	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::CheckForTakenCharacter(bool _bIsTowardsRight)
{
	//if (auto GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	//{
	//	int32 NumberOfDesiredCharacter{1};
	//	while(NumberOfDesiredCharacter >= 1)
	//	{
	//		NumberOfDesiredCharacter = GameStateCast->GetNumberOfCharactersTaken(IdealCharacter);
	//		if (NumberOfDesiredCharacter >= 1)
	//		{
	//			int32 NewColour = (int32)IdealCharacter;
	//			if (_bIsTowardsRight)
	//				NewColour ++;
	//			else
	//				NewColour --;
	//			if (NewColour > 3)
	//			{
	//				NewColour = 0;
	//			}
	//			else if (NewColour < 0)
	//			{
	//				NewColour = 3;
	//			}
	//			IdealCharacter = (ECharacters)NewColour;
	//		}
	//	}
	//}
}

void UWidget_LobbyCharacterSelection::SetCharacterColourFromSelection(int32 _NumberOfColors)
{
	if (!SkinColourData)
		return;

	IdealDetails.PureToneColour = SkinColourData->PureColours[(int16) CurrentColourSelection];

	switch(CurrentColourSelection)
	{
	case EColours::RED:
		{
			if (SkinColourData->Reds.Num() > (int16)IdealDetails.Character)
				IdealDetails.CharacterColour = SkinColourData->Reds[(int16)IdealDetails.Character];

			if (SkinColourData->Reds.Num() > (int16)IdealDetails.Character)
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

			if (SkinColourData->Whites.Num() > (int16)IdealDetails.Character)
				IdealDetails.CharacterSubColour = SkinColourData->SubWhites[(int16)IdealDetails.Character];
			break;
		}
	}
}

void UWidget_LobbyCharacterSelection::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(OwningController))
    {
		if (auto OwningPlayer = GetOwningPlayer())
		{
			OwningController = Cast<APrototype2PlayerController>(OwningPlayer);
		}
    }
}
