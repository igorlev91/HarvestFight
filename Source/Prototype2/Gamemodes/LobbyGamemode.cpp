

#include "LobbyGamemode.h"

#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"


ALobbyGamemode::ALobbyGamemode()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Player"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bUseSeamlessTravel = false;

}

void ALobbyGamemode::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyGamemode::PostLogin(APlayerController* _NewPlayer)
{
	Super::PostLogin(_NewPlayer);

	auto PlayerStateReference = _NewPlayer->GetPlayerState<ALobbyPlayerState>();
	if (!PlayerStateReference)
		return;
	
	ALobbyGamestate* GameStateReference = GetGameState<ALobbyGamestate>();
	if (!GameStateReference)
		return;
	
	auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!GameInstance)
		return;


	UE_LOG(LogTemp, Warning, TEXT("%s Joined The Game!"), *FString(PlayerStateReference->GetPlayerName()));

	GameStateReference->SetMaxPlayersOnServer(GameInstance->MaxPlayersOnServer);
	FCharacterDetails TeamAssociatedDefaultDetails{};
	if (bTeams || (GameStateReference->Server_Players.Num() <= 0 && GameInstance->bTeams))
	{
		if (GameStateReference->Server_TeamOne.Num() <= GameStateReference->Server_TeamTwo.Num())
		{
			TeamAssociatedDefaultDetails = CreateDetailsFromColourEnum(GameStateReference->TeamOneColour);
			PlayerStateReference->Details = TeamAssociatedDefaultDetails;
			GameStateReference->Server_TeamOne.Add(PlayerStateReference);
		}
		else
		{
			TeamAssociatedDefaultDetails = CreateDetailsFromColourEnum(GameStateReference->TeamTwoColour);
			PlayerStateReference->Details = TeamAssociatedDefaultDetails;
			GameStateReference->Server_TeamTwo.Add(PlayerStateReference);
		}
		UE_LOG(LogTemp, Warning, TEXT("Player %s Joined Team %s"), *FString::FromInt(GameStateReference->Server_Players.Num()), *FString::FromInt((int)PlayerStateReference->Details.Colour));
	}

	GameStateReference->Server_Players.Add(PlayerStateReference);
	
	UpdateAllPlayerInfo(GameStateReference, GameInstance);
}

void ALobbyGamemode::Logout(AController* _Exiting)
{
	Super::Logout(_Exiting);

	ALobbyPlayerState* PlayerStateReference = _Exiting->GetPlayerState<ALobbyPlayerState>();
	if (!PlayerStateReference)
		return;
	
	ALobbyGamestate* LobbyGamestate = GetGameState<ALobbyGamestate>();
	if (!LobbyGamestate)
		return;

	UPrototypeGameInstance* PrototypeGameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!PrototypeGameInstance)
		return;

	LobbyGamestate->Server_Players.Remove(PlayerStateReference);
	LobbyGamestate->Server_TeamOne.Remove(PlayerStateReference);
	LobbyGamestate->Server_TeamTwo.Remove(PlayerStateReference);
	if (PrototypeGameInstance->FinalPlayerDetails.Contains(PlayerStateReference->PlayerName))
	{
		PrototypeGameInstance->FinalPlayerDetails.Remove(PlayerStateReference->PlayerName);
	}
	
	UpdateAllPlayerInfo(LobbyGamestate, PrototypeGameInstance);
}

void ALobbyGamemode::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
}

void ALobbyGamemode::UpdateAllPlayerInfo(ALobbyGamestate* _GameStateReference, UPrototypeGameInstance* _gameInstanceReference)
{
	for(int32 i = 0; i < _GameStateReference->Server_Players.Num(); i++)
	{
		int32 SomePlayerID = i;
		FString SomePlayerName{};
		
		if (auto SteamSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM))
		{
			IOnlineIdentityPtr IdentityInterface = SteamSubsystem->GetIdentityInterface();
			if (IdentityInterface.IsValid())
			{
				auto UserId = _GameStateReference->Server_Players[i]->GetUniqueId().GetUniqueNetId();
				if (UserId.IsValid())
				{
					SomePlayerName = _GameStateReference->Server_Players[i]->GetPlayerName();
					_gameInstanceReference->FinalPlayerDetails.FindOrAdd(UserId->ToString()) = _GameStateReference->Server_Players[i]->Details;
				}
			}
		}
		else if (auto NullSubsystem = IOnlineSubsystem::Get())
		{
			IOnlineIdentityPtr IdentityInterface = NullSubsystem->GetIdentityInterface();
			if (IdentityInterface.IsValid())
			{
				auto UserId = _GameStateReference->Server_Players[i]->GetUniqueId().GetUniqueNetId();
				if (UserId.IsValid())
				{
					SomePlayerName = "Player " + FString::FromInt(SomePlayerID + 1);
					_gameInstanceReference->FinalPlayerDetails.FindOrAdd(UserId->ToString()) = _GameStateReference->Server_Players[i]->Details;
				}
			}
		}
		
		if (ALobbyPlayerState* CasterPlayerState = Cast<ALobbyPlayerState>(_GameStateReference->Server_Players[i]))
		{
			CasterPlayerState->Player_ID = SomePlayerID;
			CasterPlayerState->PlayerName = SomePlayerName;

			/* Limit player name if too long and adding ... to the end */
			if (CasterPlayerState->PlayerName.Len() >= 10)
			{
				CasterPlayerState->PlayerName = CasterPlayerState->PlayerName.Left(FMath::Min(CasterPlayerState->PlayerName.Len(), 8));
				CasterPlayerState->PlayerName = CasterPlayerState->PlayerName + "...";
			}
		}
	}
}


FCharacterDetails ALobbyGamemode::CreateDetailsFromColourEnum(EColours _Colour)
{
	FCharacterDetails IdealDetails{};
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


