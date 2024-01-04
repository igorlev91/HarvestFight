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
	CheckForTakenSkin(true);
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
	switch(IdealCharacter)
	{
	case ECharacters::COW:
		{
			if (CowTextures.Num() > 0)
			{
				PlayerImage->SetBrushFromTexture(CowTextures[(int)IdealCharacterColour]);
			}
			break;
		}
	case ECharacters::CHICKEN:
		{
			if (ChickenTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(ChickenTextures[(int)IdealCharacterColour]);
			break;
		}
	case ECharacters::DUCK:
		{
			if (DuckTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(DuckTextures[(int)IdealCharacterColour]);
			break;
		}
	case ECharacters::PIG:
		{
			if (PigTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(PigTextures[(int)IdealCharacterColour]);
			break;
		}
	default:
		{
			if (CowTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(CowTextures[(int)IdealCharacterColour]);
			break;
		}
	}

	switch(IdealCharacterColour)
	{
	case ECharacterColours::RED:
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
		break;
	case ECharacterColours::BLUE:
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.034125f, 0.118703f, 0.192708f, 1.0f));
		break;
	case ECharacterColours::GREEN:
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.078957f, 0.192708f, 0.056207f, 1.0f));
		break;
	case ECharacterColours::YELLOW:
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.317708f, 0.180145, 0.033095, 1.0f));
		break;
	default:
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
		break;
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
			else if (newColour < 0)
			{
				
				newColour = 3;
			}
			
			IdealCharacterColour = (ECharacterColours)newColour;
		}
	}

	// check for taken sskin
	CheckForTakenSkin(_right);
	
	UpdateCharacterImage();

	if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		gameInstance->Character = IdealCharacter;
		gameInstance->CharacterColour = IdealCharacterColour;
	}
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _right)
{
	if (auto playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto playerState = playerController->GetPlayerState<ALobbyPlayerState>())
		{
			int newCharacter = (int)IdealCharacter;
			if (_right)
			{
				newCharacter ++;
			}
			else
			{
				newCharacter --;
			}
	
			if (newCharacter > 3)
			{
				newCharacter = 0;
			}
			else if (newCharacter < 0)
			{
				
				newCharacter = 3;
			}
			
			IdealCharacter = ECharacters(newCharacter);
		}
	}

	// check for taken sskin
	CheckForTakenCharacter(_right);

	if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		gameInstance->Character = IdealCharacter;
		gameInstance->CharacterColour = IdealCharacterColour;
	}
	
	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::CheckForTakenSkin(bool _right)
{
	if (auto gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (auto playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
		{
			if (auto playerState = playerController->GetPlayerState<ALobbyPlayerState>())
			{
				int numOfDesiredCharacterColours{1};
				while(numOfDesiredCharacterColours >= 1)
				{
					numOfDesiredCharacterColours = gameState->GetNumberOfCharacterColoursTaken(IdealCharacter, IdealCharacterColour);
					if (numOfDesiredCharacterColours >= 1)
					{
						int newColour = (int)IdealCharacterColour;
						if (_right)
							newColour ++;
						else
							newColour --;
						if (newColour > 3)
						{
							newColour = 0;
						}
						else if (newColour < 0)
						{
							newColour = 3;
						}
						IdealCharacterColour = (ECharacterColours)newColour;
					}
				}
				//UE_LOG(LogTemp, Warning, TEXT("Number of desired Character Colours: %s"), *FString::FromInt(numOfDesiredCharacterColours));

				auto playerID = playerState->Player_ID;
				playerController->UpdateCharacterMaterial(playerID, IdealCharacter, IdealCharacterColour);
			}
		}
	}
}

void UWidget_LobbyCharacterSelection::CheckForTakenCharacter(bool _right)
{
	if (auto gameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (auto playerController = Cast<APrototype2PlayerController>(GetOwningPlayer()))
		{
			if (auto playerState = playerController->GetPlayerState<ALobbyPlayerState>())
			{
				int numOfDesiredCharacter{1};
				while(numOfDesiredCharacter >= 1)
				{
					numOfDesiredCharacter = gameState->GetNumberOfCharacterColoursTaken(IdealCharacter, IdealCharacterColour);
					if (numOfDesiredCharacter >= 1)
					{
						int newColour = (int)IdealCharacter;
						if (_right)
							newColour ++;
						else
							newColour --;
						if (newColour > 3)
						{
							newColour = 0;
						}
						else if (newColour < 0)
						{
							newColour = 3;
						}
						IdealCharacter = (ECharacters)newColour;
					}
				}
				//UE_LOG(LogTemp, Warning, TEXT("Number of desired Character Colours: %s"), *FString::FromInt(numOfDesiredCharacterColours));

				auto playerID = playerState->Player_ID;
				playerController->UpdateCharacterMaterial(playerID, IdealCharacter, IdealCharacterColour);
			}
		}
	}
}

void UWidget_LobbyCharacterSelection::UpdateCharacterColourImage(ECharacters _desiredCharacter)
{
	
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
				CheckForTakenSkin(true);
				
				UpdateCharacterImage();

				if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
				{
					gameInstance->Character = IdealCharacter;
					gameInstance->CharacterColour = IdealCharacterColour;
				}
			}
		}
	}
}
