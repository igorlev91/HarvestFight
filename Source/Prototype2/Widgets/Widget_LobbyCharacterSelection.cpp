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
	
	CheckForTakenCharacter(true);
	if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
		{
			auto PlayerID = PlayerState->Player_ID;
			PlayerControllerCast->UpdateCharacterMaterial(PlayerID, IdealCharacter, IdealCharacterColour);
		}
	}
	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	switch(IdealCharacter)
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

	PlayerColourImage->SetColorAndOpacity((FLinearColor)IdealCharacterColour);
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColour(bool _bIsTowardsRight)
{
	//if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	//{
	//	if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
	//	{
	//		int32 NewColour = (int32)IdealCharacterColour;
	//		if (_bIsTowardsRight)
	//		{
	//			NewColour ++;
	//		}
	//		else
	//		{
	//			NewColour --;
	//		}
	//
	//		if (NewColour > 3)
	//		{
	//			NewColour = 0;
	//		}
	//		else if (NewColour < 0)
	//		{
	//			
	//			NewColour = 3;
	//		}
	//		
	//		IdealCharacterColour = (ECharacterColours)NewColour;
	//	}
	//}

	// check for taken sskin
	CheckForTakenSkin(_bIsTowardsRight);
	
	UpdateCharacterImage();

	if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		GameInstance->FinalPlayerDetails.FindOrAdd(GameInstance->UniqueNetIDName, {IdealCharacter, IdealCharacterColour});
	}
}

void UWidget_LobbyCharacterSelection::ChangeCharacter(bool _bIsTowardsRight)
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

	// Check for taken skin
	CheckForTakenCharacter(_bIsTowardsRight);
}

void UWidget_LobbyCharacterSelection::CheckForTakenSkin(bool _bIsTowardsRight)
{
	if (auto PlayerControllerCast = Cast<APrototype2PlayerController>(GetOwningPlayer()))
	{
		if (auto PlayerState = PlayerControllerCast->GetPlayerState<ALobbyPlayerState>())
		{
			auto PlayerID = PlayerState->Player_ID;
			PlayerControllerCast->UpdateCharacterMaterial(PlayerID, IdealCharacter, IdealCharacterColour);
		}
	}
}

void UWidget_LobbyCharacterSelection::CheckForTakenCharacter(bool _bIsTowardsRight)
{
	if (auto GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		int32 NumberOfDesiredCharacter{1};
		while(NumberOfDesiredCharacter >= 1)
		{
			NumberOfDesiredCharacter = GameStateCast->GetNumberOfCharactersTaken(IdealCharacter);
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
				if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
				{
					if (PlayerControllerCast->HasAuthority())
					{
						FString Name{};
						IOnlineIdentityPtr IdentityInterface = IOnlineSubsystem::Get()->GetIdentityInterface();
						if (IdentityInterface.IsValid())
						{
							Name = IdentityInterface->GetPlayerNickname(*PlayerState->GetUniqueId().GetUniqueNetId());
						}
						FCharacterDetails Details{};
						Details.Character = IdealCharacter;
						Details.CharacterColour = IdealCharacterColour;
					
						if (GameInstance->FinalPlayerDetails.Contains(Name))
							GameInstance->FinalPlayerDetails[Name] = Details;
						else
							GameInstance->FinalPlayerDetails.Add(Name, Details);
					}
				}

				auto PlayerID = PlayerState->Player_ID;
				PlayerControllerCast->UpdateCharacterMaterial(PlayerID, IdealCharacter, IdealCharacterColour);
				UpdateCharacterImage();
			}
		}
	}
}
