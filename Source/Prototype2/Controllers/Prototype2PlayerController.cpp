

#include "Prototype2PlayerController.h"

#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Prototype2/Gameplay/Endgame/EndGamePodium.h"
#include "Prototype2/Widgets/Widget_PlayerHUD.h"
#include "Prototype2//Gamestates/Prototype2Gamestate.h"

void APrototype2PlayerController::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
	if (auto gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gamestate;
	}

	bCanPossessWithoutAuthority = true;
}

void APrototype2PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GameStateRef)
		return;

	//UE_LOG(LogTemp, Warning, TEXT("HasGameStarted? : %s"), *FString::FromInt((int)GameStateRef->GameHasStarted));

	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		if (!GameStateRef->HasGameFinished() && GameStateRef->HasGameStarted())
		{
			if (bEnableMovement == false)
			{
				SetInputMode(FInputModeGameOnly());
				bEnableMovement = true;
			}
		}
		
	
		if (auto gameInstance = GetGameInstance<UPrototypeGameInstance>())
		{
			if (auto playerState = GetPlayerState<ALobbyPlayerState>())
			{
				UpdateCharacterMaterial(playerState->Player_ID, gameInstance->Character, gameInstance->CharacterColour);
			}
		}
	}
}

void APrototype2PlayerController::SetIsReady(int32 _Player, bool _bIsReady)
{
	Server_SetIsReady(_Player, _bIsReady);
}

void APrototype2PlayerController::Server_SetIsReady_Implementation(int32 _Player, bool _bIsReady)
{
	if (auto* GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateCast->SetIsReady(_Player, _bIsReady);
	}
}

void APrototype2PlayerController::VoteMap(int32 _Player, EFarm _Level)
{
	Server_VoteMap(_Player, _Level);
}


void APrototype2PlayerController::Server_VoteMap_Implementation(int32 _Player, EFarm _Level)
{
	if (auto* GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateCast->VoteMap(_Level);
	}
}

void APrototype2PlayerController::UpdateCharacterMaterial(int32 _Player, ECharacters _Character, ECharacterColours _CharacterColour)
{
	Server_UpdateCharacterMaterial(_Player, _Character, _CharacterColour);
}

void APrototype2PlayerController::Server_UpdateCharacterMaterial_Implementation(int32 _Player, ECharacters _Character,
	ECharacterColours _CharacterColour)
{
	if (auto GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateCast->UpdateCharacterMaterial(_Player, _Character, _CharacterColour);
	}
}

void APrototype2PlayerController::KickFromLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMenu")), true, "kicked");
}
