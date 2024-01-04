// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyCharacterSelection.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/LobbyCharacter.h"
#include "Prototype2/LobbyPlayerState.h"
#include "Prototype2/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

void UWidget_LobbyCharacterSelection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayerImage->SetBrushFromTexture(Texture_CowRed);
	PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
	CheckForTakenSkin();
}

void UWidget_LobbyCharacterSelection::ChangeCharacterLeft()
{
	
}

void UWidget_LobbyCharacterSelection::ChangeCharacterRight()
{
	
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColourLeft()
{
	//if (auto gameInstance = Cast<UPrototypeGameInstance>(GetGameInstance()))
	//{
	//	switch(gameInstance->CharacterColour)
	//	{
	//	case ECharacterColours::RED:
	//		gameInstance->CharacterColour = ECharacterColours::YELLOW;
	//		break;
	//	case ECharacterColours::BLUE:
	//		gameInstance->CharacterColour = ECharacterColours::RED;
	//		break;
	//	case ECharacterColours::GREEN:
	//		gameInstance->CharacterColour = ECharacterColours::BLUE;
	//		break;
	//	case ECharacterColours::YELLOW:
	//		gameInstance->CharacterColour = ECharacterColours::GREEN;
	//		break;
	//	default:
	//		gameInstance->CharacterColour = ECharacterColours::RED;
	//		break;
	//	}
	//}
//
	//UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColourRight()
{
	//if (auto gameInstance = Cast<UPrototypeGameInstance>(GetGameInstance()))
	//{
	//	switch(gameInstance->CharacterColour)
	//	{
	//	case ECharacterColours::RED:
	//		gameInstance->CharacterColour = ECharacterColours::BLUE;
	//		break;
	//	case ECharacterColours::BLUE:
	//		gameInstance->CharacterColour = ECharacterColours::GREEN;
	//		break;
	//	case ECharacterColours::GREEN:
	//		gameInstance->CharacterColour = ECharacterColours::YELLOW;
	//		break;
	//	case ECharacterColours::YELLOW:
	//		gameInstance->CharacterColour = ECharacterColours::RED;
	//		break;
	//	default:
	//		gameInstance->CharacterColour = ECharacterColours::RED;
	//		break;
	//	}
	//}
//
	//UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	if (auto playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto playerState = playerController->GetPlayerState<ALobbyPlayerState>())
		{
			switch(IdealCharacterColour)
			{
			case ECharacterColours::RED:
				PlayerImage->SetBrushFromTexture(Texture_CowRed);
				PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
				break;
			case ECharacterColours::BLUE:
				PlayerImage->SetBrushFromTexture(Texture_CowBlue);
				PlayerColourImage->SetColorAndOpacity(FLinearColor(0.034125f, 0.118703f, 0.192708f, 1.0f));
				break;
			case ECharacterColours::GREEN:
				PlayerImage->SetBrushFromTexture(Texture_CowGreen);
				PlayerColourImage->SetColorAndOpacity(FLinearColor(0.078957f, 0.192708f, 0.056207f, 1.0f));
				break;
			case ECharacterColours::YELLOW:
				PlayerImage->SetBrushFromTexture(Texture_CowYellow);
				PlayerColourImage->SetColorAndOpacity(FLinearColor(0.317708f, 0.180145, 0.033095, 1.0f));
				break;
			default:
				PlayerImage->SetBrushFromTexture(Texture_CowRed);
				PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
				break;
			}
		}
	}
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColour(bool _right)
{
	if (auto playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto playerState = playerController->GetPlayerState<ALobbyPlayerState>())
		{
			int newColour = (int)IdealCharacterColour;
			if (_right)
			{
				newColour ++;
			
			}
			else
			{
				newColour --;
			}
	
			if (newColour > 3)
			{
				newColour = 0;
			}
			else
			{
				if (newColour < 0)
				{
					newColour = 3;
				}
			}
			
			IdealCharacter = ECharacters::COW;
			IdealCharacterColour = (ECharacterColours)newColour;
		}
	}
	
	CheckForTakenSkin();
}

void UWidget_LobbyCharacterSelection::CheckForTakenSkin()
{
	if (auto gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		int numOfDesiredCharacter{4};
		while(numOfDesiredCharacter >= 4)
		{
			numOfDesiredCharacter = gameState->GetNumberOfCharactersTaken(IdealCharacter);
			if (numOfDesiredCharacter >= 4)
			{
				int newCharacter = (int)IdealCharacter;
				newCharacter ++;
				if (newCharacter > 3)
				{
					newCharacter = 0;
				}
				else
				{
					if (newCharacter < 0)
					{
						newCharacter = 3;
					}
				}
				IdealCharacter = (ECharacters)newCharacter;
			}
		}

		int numOfDesiredCharacterColours{4};
		while(numOfDesiredCharacterColours >= 4)
		{
			numOfDesiredCharacterColours = gameState->GetNumberOfCharacterColoursTaken(IdealCharacter, IdealCharacterColour);
			if (numOfDesiredCharacterColours >= 4)
			{
				int newColour = (int)IdealCharacterColour;
				newColour ++;
				if (newColour > 3)
				{
					newColour = 0;
				}
				else
				{
					if (newColour < 0)
					{
						newColour = 3;
					}
				}
				IdealCharacterColour = (ECharacterColours)newColour;
			}
		}
	}
	
	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (auto playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto playerState = playerController->GetPlayerState<ALobbyPlayerState>())
		{
			if (playerState->Character != IdealCharacter || playerState->CharacterColour != IdealCharacterColour)
			{
				auto playerID = playerState->Player_ID;
				playerController->UpdateCharacterMaterial(playerID, IdealCharacter, IdealCharacterColour);
			}
		}
	}
}
