

#include "HHGamemode.h"

#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/DataAssets/RandomNameData.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"

AHHGamemodeBase::AHHGamemodeBase()
{
	static ConstructorHelpers::FObjectFinder<URandomNameData> RandomLANNameData(TEXT("/Game/DataAssets/MiscData/DA_RandomNames"));
	if (RandomLANNameData.Object != NULL)
	{
		RandomNameData = RandomLANNameData.Object;
	}
}

void AHHGamemodeBase::UpdateSessionJoinability(int32 _MaxPlayers)
{
	if (GameSession)
	{
		FJoinabilitySettings OutSettings;
		GameSession->GetSessionJoinability(NAME_GameSession, OutSettings);
		GameSession->MaxPlayers = _MaxPlayers;
		GameSession->UpdateSessionJoinability(NAME_GameSession,
			OutSettings.bPublicSearchable, OutSettings.bAllowInvites, 
			OutSettings.bJoinViaPresence, OutSettings.bJoinViaPresenceFriendsOnly);

		if (auto Subsystem = IOnlineSubsystem::Get())
		{
			if (auto Session = Subsystem->GetSessionInterface())
			{
				Session->UpdateSession(NAME_GameSession, *Session->GetSessionSettings(NAME_GameSession), true);
				UE_LOG(LogTemp, Warning, TEXT("Session Refreshed!"));
			}
		}
	}
}

FCharacterDetails AHHGamemodeBase::CreateDetailsFromColourEnum(EColours _Colour, bool _RandomCharacter)
{
	FCharacterDetails IdealDetails{};

	if (AvailableNames.Num() > 0)
	{
		IdealDetails.RandomizedName = AvailableNames[rand() % AvailableNames.Num()];
		AvailableNames.Remove(IdealDetails.RandomizedName);
	}
	else if (IsValid(RandomNameData))
	{
		AvailableNames = RandomNameData->RandomNames;
		if (AvailableNames.Num() > 0)
		{
			IdealDetails.RandomizedName = AvailableNames[rand() % AvailableNames.Num()];
			AvailableNames.Remove(IdealDetails.RandomizedName);
		}
	}
	
	if (UPrototypeGameInstance* SomeGameInstance = Cast<UPrototypeGameInstance>(GetGameInstance()))
	{
		if (_RandomCharacter)
		{
			auto RandomCharacter = rand() % (SomeGameInstance->PlayerModels.Num());
			IdealDetails.AnimationData = SomeGameInstance->PlayerModels[RandomCharacter];
			IdealDetails.Character = (ECharacters)RandomCharacter;
		}
		else
		{
			IdealDetails.AnimationData = SomeGameInstance->PlayerModels[0];
		}
	}
	
	IdealDetails.Colour = _Colour;
	IdealDetails.PureToneColour = SkinColourData->PureColours[(int16) IdealDetails.Colour];

	switch(_Colour)
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

	return IdealDetails;
}

EColours AHHGamemodeBase::GetFirstFreeColor(APlayerState* InPlayerState)
{
	ALobbyGamestate* LobbyGamestate = GetGameState<ALobbyGamestate>();
	if (IsValid(LobbyGamestate))
	{
		TArray<EColours> AvailableColours
		{
			EColours::RED,
			EColours::BLUE,
			EColours::GREEN,
			EColours::YELLOW,
			EColours::PURPLE,
			EColours::ORANGE,
			EColours::BLACK,
			EColours::WHITE
		};
	
		for(ALobbyPlayerState* SomePlayerState : LobbyGamestate->Server_Players)
		{
			AvailableColours.Remove(SomePlayerState->Details.Colour);
		}

		if (AvailableColours.Num() > 0)
			return AvailableColours[rand() % AvailableColours.Num()];
	}

	APrototype2Gamestate* NormalGamestate = GetGameState<APrototype2Gamestate>();
	if (IsValid(NormalGamestate))
	{
		TArray<EColours> AvailableColours
		{
			EColours::RED,
			EColours::BLUE,
			EColours::GREEN,
			EColours::YELLOW,
			EColours::PURPLE,
			EColours::ORANGE,
			EColours::BLACK,
			EColours::WHITE
		};
	
		for(APrototype2PlayerState* SomePlayerState : NormalGamestate->Server_Players)
		{
			if (SomePlayerState != Cast<APrototype2PlayerState>(InPlayerState))
				AvailableColours.Remove(SomePlayerState->Details.Colour);
		}

		if (AvailableColours.Num() > 0)
			return AvailableColours[rand() % AvailableColours.Num()];
	}

	return EColours::WHITE;
}
