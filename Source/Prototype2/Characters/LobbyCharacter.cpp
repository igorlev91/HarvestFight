

#include "LobbyCharacter.h"

#include "Components/TextBlock.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Components/WidgetComponent.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Widgets/Widget_LobbyCharacterSelection.h"
#include "Prototype2/Widgets/Widget_LobbyPlayerHUDV2.h"
#include "Prototype2/HUDS/LobbyHUD.h"
#include "Prototype2/Widgets/Widget_PlayerName.h"

ALobbyCharacter::ALobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ALobbyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (IsLocallyControlled())
	{
		auto PlayerController = Cast<APlayerController>(Controller);
		if (IsValid(PlayerController) == false)
			return;

		auto LobbyHUD = PlayerController->GetHUD<ALobbyHUD>();
		if (IsValid(LobbyHUD) == false)
			return;

		if (LobbyHUD->IsChangingCharacterSkin())
		{
			GetMesh()->SetVisibility(false);
		}
		else
		{
			GetMesh()->SetVisibility(true);
			SyncCharacterSkin();
		}
	}
	else
	{
		GetMesh()->SetVisibility(false);
	}
}

void ALobbyCharacter::SyncCharacterSkin()
{
	UPrototypeGameInstance* SomeGameInstance = Cast<UPrototypeGameInstance>(GetGameInstance());
	ALobbyPlayerState* SomePlayerState = GetPlayerState<ALobbyPlayerState>();
	if (IsValid(SomePlayerState) == false)
		return;

	FCharacterDetails CurrentDetails = SomePlayerState->Details;
	
	GetMesh()->SetSkeletalMeshAsset(SomeGameInstance->LobbyPlayerModels[(int16)CurrentDetails.Character]);
	
	if (IsValid(SomeGameInstance->LobbyAnimBP))
		GetMesh()->SetAnimInstanceClass(SomeGameInstance->LobbyAnimBP);
	
	SomeGameInstance->PlayerMaterialsDynamic[(int16)CurrentDetails.Character]->SetVectorParameterValue(FName("Cow Colour"), CurrentDetails.CharacterColour);
	SomeGameInstance->PlayerMaterialsDynamic[(int16)CurrentDetails.Character]->SetVectorParameterValue(FName("Spot Colour"), CurrentDetails.CharacterSubColour);
	
	GetMesh()->SetMaterial(0, SomeGameInstance->PlayerMaterialsDynamic[(int16)CurrentDetails.Character]);
}

void ALobbyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ALobbyPlayerState* SomePlayerState = GetPlayerState<ALobbyPlayerState>();
	if (IsValid(SomePlayerState) == false)
		return;

	if (IsValid(HUD) == false)
		return;

	HUD->OnRep_CharacterDetails(SomePlayerState->Details);
}



