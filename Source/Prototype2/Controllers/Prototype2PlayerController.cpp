

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
}

void APrototype2PlayerController::SetIsReady(int32 _Player, bool _bIsReady)
{
	Server_SetIsReady(_Player, _bIsReady);
}

void APrototype2PlayerController::SetViewTarget_Networked(AActor* _ViewTarget)
{
	if (HasAuthority())
	{
		Multi_SetViewTarget_Networked(_ViewTarget);
	}
	else
	{
		Server_SetViewTarget_Networked(_ViewTarget);
	}
}

void APrototype2PlayerController::Server_SetViewTarget_Networked_Implementation(AActor* _ViewTarget)
{
	Multi_SetViewTarget_Networked(_ViewTarget);
}

void APrototype2PlayerController::Multi_SetViewTarget_Networked_Implementation(AActor* _ViewTarget)
{
	SetControlRotation({});
	//SetInputMode(FInputModeUIOnly{});
	bShowMouseCursor = true;
	SetViewTargetWithBlend(_ViewTarget, 0.75f);
}

void APrototype2PlayerController::Server_SetIsReady_Implementation(int32 _Player, bool _bIsReady)
{
	//if (ALobbyGamestate* GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	//{
	//	GameStateCast->SetIsReady(_Player, _bIsReady);
	//}
}

void APrototype2PlayerController::VoteMap(EFarm _Level)
{
	if (HasAuthority())
	{
		
	}
	else
	{
		Server_VoteMap(_Level);
	}
}


void APrototype2PlayerController::Server_VoteMap_Implementation(EFarm _Level)
{
	if (ALobbyGamestate* LobbyGameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (LobbyGameStateCast)
			LobbyGameStateCast->VoteMap(_Level);
	}

	if (APrototype2Gamestate* Proto2GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if (Proto2GameStateCast)
			Proto2GameStateCast->VoteMap(_Level);

		UE_LOG(LogTemp, Warning, TEXT("Prototype2 gamestate vote map function called"));
	}
}

void APrototype2PlayerController::UpdateCharacterMaterial(int32 _Player, FCharacterDetails _Details)
{
	Server_UpdateCharacterMaterial(_Player, _Details);
}

void APrototype2PlayerController::Server_UpdateCharacterMaterial_Implementation(int32 _Player, FCharacterDetails _Details)
{
	//if (ALobbyGamestate* GameStateCast = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	//{
	//	GameStateCast->UpdatePlayerDetails(_Player, _Details);
	//}
	if (APrototype2Gamestate* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateCast->UpdatePlayerDetails(_Player, _Details);
	}
}

//void APrototype2PlayerController::Multi_UpdateCharacterMaterial_Implementation(FCharacterDetails _Details)
//{
//	if (auto GameMode = UGameplayStatics::GetGameMode(GetWorld()))
//	{
//		auto CastedGameMode = Cast<APrototype2GameMode>(GameMode);
//		auto CastedLobbyGameMode = Cast<ALobbyGamemode>(GameMode);
//		
//		if (CastedLobbyGameMode || CastedGameMode)
//		{
//			if (CastedGameMode && CastedGameMode->PlayerMeshes.Num() > (int32)_Details.Character)
//			{
//				GetCharacter()->GetMesh()->SetSkeletalMeshAsset(
//				CastedLobbyGameMode ? CastedLobbyGameMode->PlayerMeshes[(int32)_Details.Character]
//				:
//				CastedGameMode->PlayerMeshes[(int32)_Details.Character]);
//			}
//
//			if (CastedLobbyGameMode && CastedLobbyGameMode->PlayerMaterials.Num() > (int32)_Details.Character)
//			{
//				GetCharacter()->GetMesh()->SetMaterial(0,
//					CastedLobbyGameMode ? CastedLobbyGameMode->PlayerMaterials[(int32)_Details.Character]
//					:
//					CastedGameMode->PlayerMaterials[(int32)_Details.Character]);
//			}
//		}
//	}
//	
//	auto MaterialInstance = GetCharacter()->GetMesh()->CreateDynamicMaterialInstance(0);
//	auto SkinColour = _Details.CharacterColour;
//	auto FeaturesColour = _Details.CharacterColour / 1.5f;
//	// chicken??
//	MaterialInstance->SetVectorParameterValue(FName("Cow Colour"), SkinColour);
//	MaterialInstance->SetVectorParameterValue(FName("Spot Colour"), FeaturesColour);
//
//	GetCharacter()->GetMesh()->SetMaterial(0, MaterialInstance);
//}

void APrototype2PlayerController::SyncPlayerMaterial(int32 _PlayerID, FCharacterDetails _CharacterDetails)
{
	//UpdateCharacterMaterial(_PlayerID, _CharacterDetails);
	
	UE_LOG(LogTemp, Warning, TEXT("Update Player %s Skin (%s)"), *FString::FromInt(_PlayerID), *_CharacterDetails.CharacterColour.ToString());
	


	auto LobbyPlayerState = GetPlayerState<ALobbyPlayerState>();
	auto GamePlayerState = GetPlayerState<APrototype2PlayerState>();
	
	FString Name{};
	IOnlineIdentityPtr IdentityInterface = IOnlineSubsystem::Get()->GetIdentityInterface();
	if (!IdentityInterface.IsValid())
		return;

	if (LobbyPlayerState)
	{
		if (auto NetID = LobbyPlayerState->GetUniqueId().GetUniqueNetId())
		{
			Name = NetID->ToString();
		}
	}
	else if (GamePlayerState)
	{
		if (auto NetID = GamePlayerState->GetUniqueId().GetUniqueNetId())
		{
			Name = NetID->ToString();
		}
	}

	if (HasAuthority())
	{
		if (auto GameInstance = GetGameInstance<UPrototypeGameInstance>())
		{
			if (GameInstance->FinalPlayerDetails.Contains(Name))
				GameInstance->FinalPlayerDetails[Name] = _CharacterDetails;
			else
				GameInstance->FinalPlayerDetails.Add(Name, _CharacterDetails);
		}
	}
}

void APrototype2PlayerController::Server_UpdatePlayerDetails_Implementation(int32 _Player, FCharacterDetails _CharacterDetails, const FString& _PlayerName)
{
	if (UPrototypeGameInstance* GameInstance = GetGameInstance<UPrototypeGameInstance>())
	{
		if (GameInstance->FinalPlayerDetails.Contains(_PlayerName))
			GameInstance->FinalPlayerDetails[_PlayerName] = _CharacterDetails;
		else
			GameInstance->FinalPlayerDetails.Add(_PlayerName, _CharacterDetails);
	}
}

void APrototype2PlayerController::KickFromLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMenu")), true, "kicked");
}


