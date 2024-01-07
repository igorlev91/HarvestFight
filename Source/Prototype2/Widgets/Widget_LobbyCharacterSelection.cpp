// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyCharacterSelection.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

void UWidget_LobbyCharacterSelection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayerImage->SetBrushFromTexture(Texture_CowRed);
	PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
	CheckForTakenSkin(true);
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	switch(IdealCharacter)
	{
	case ECharacters::COW:
		{
			if (CowTextures.Num() > 0)
			{
				PlayerImage->SetBrushFromTexture(CowTextures[(int32)IdealCharacterColour]);
			}
			break;
		}
	case ECharacters::CHICKEN:
		{
			if (ChickenTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(ChickenTextures[(int32)IdealCharacterColour]);
			break;
		}
	case ECharacters::DUCK:
		{
			if (DuckTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(DuckTextures[(int32)IdealCharacterColour]);
			break;
		}
	case ECharacters::PIG:
		{
			if (PigTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(PigTextures[(int32)IdealCharacterColour]);
			break;
		}
	default:
		{
			if (CowTextures.Num() > 0)
				PlayerImage->SetBrushFromTexture(CowTextures[(int32)IdealCharacterColour]);
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

void UWidget_LobbyCharacterSelection::ChangeCharacterColour(bool _bIsTowardsRight)
{
	if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
		{
			int32 NewColour = (int32)IdealCharacterColour;
			if (_bIsTowardsRight)
			{
				NewColour ++;
			}
			else
			{
				NewColour --;
			}
	
			if (NewColour > 3)
			{
				NewColour = 0;
			}
			else if (NewColour < 0)
			{
				
				NewColour = 3;
			}
			
			IdealCharacterColour = (ECharacterColours)NewColour;
		}
	}

	// check for taken sskin
	CheckForTakenSkin(_bIsTowardsRight);
	
	UpdateCharacterImage();

	if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		GameInstance->Character = IdealCharacter;
		GameInstance->CharacterColour = IdealCharacterColour;
					
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character: %s"), *FString::FromInt((int32)GameInstance->Character));
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character Colour: %s"), *FString::FromInt((int32)GameInstance->CharacterColour));
	}
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _bIsTowardsRight)
{
	if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
		{
			int32 NewCharacter = (int32)IdealCharacter;
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
			
			IdealCharacter = ECharacters(NewCharacter);
		}
	}

	// Check for taken skin
	CheckForTakenCharacter(_bIsTowardsRight);
	
	UpdateCharacterImage();
	
	if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		GameInstance->Character = IdealCharacter;
		GameInstance->CharacterColour = IdealCharacterColour;
					
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character: %s"), *FString::FromInt((int32)GameInstance->Character));
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Character Colour: %s"), *FString::FromInt((int32)GameInstance->CharacterColour));
	}
}

void UWidget_LobbyCharacterSelection::CheckForTakenSkin(bool _bIsTowardsRight)
{
	if (auto GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
		{
			if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
			{
				int32 NumberOfDesiredCharacterColours{1};
				while(NumberOfDesiredCharacterColours >= 1)
				{
					NumberOfDesiredCharacterColours = GameStateCast->GetNumberOfCharacterColoursTaken(IdealCharacter, IdealCharacterColour);
					if (NumberOfDesiredCharacterColours >= 1)
					{
						int32 NewColour = (int32)IdealCharacterColour;
						if (_bIsTowardsRight)
							NewColour ++;
						else
							NewColour --;
						if (NewColour > 3)
						{
							NewColour = 0;
						}
						else if (NewColour < 0)
						{
							NewColour = 3;
						}
						IdealCharacterColour = (ECharacterColours)NewColour;
					}
				}

				auto PlayerID = PlayerState->Player_ID;
				PlayerControllerCast->UpdateCharacterMaterial(PlayerID, IdealCharacter, IdealCharacterColour);
			}
		}
	}
}

void UWidget_LobbyCharacterSelection::CheckForTakenCharacter(bool _bIsTowardsRight)
{
	if (auto GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
		{
			if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
			{
				int32 NumberOfDesiredCharacter{1};
				while(NumberOfDesiredCharacter >= 1)
				{
					NumberOfDesiredCharacter = GameStateCast->GetNumberOfCharacterColoursTaken(IdealCharacter, IdealCharacterColour);
					if (NumberOfDesiredCharacter >= 1)
					{
						int32 NewColour = (int32)IdealCharacter;
						if (_bIsTowardsRight)
							NewColour ++;
						else
							NewColour --;
						if (NewColour > 3)
						{
							NewColour = 0;
						}
						else if (NewColour < 0)
						{
							NewColour = 3;
						}
						IdealCharacter = (ECharacters)NewColour;
					}
				}

				auto PlayerID = PlayerState->Player_ID;
				PlayerControllerCast->UpdateCharacterMaterial(PlayerID, IdealCharacter, IdealCharacterColour);
			}
		}
	}
}

void UWidget_LobbyCharacterSelection::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
		{
			if (PlayerState->Character != IdealCharacter || PlayerState->CharacterColour != IdealCharacterColour)
			{
				CheckForTakenSkin(true);

				if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
				{
					GameInstance->Character = IdealCharacter;
					GameInstance->CharacterColour = IdealCharacterColour;
				}
				
				UpdateCharacterImage();
			}
		}
	}
}
