

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

	if (!IsLocallyControlled())
		GetMesh()->SetVisibility(false);
}

void ALobbyCharacter::SyncCharacterSkin()
{
	//if (PlayerStateRef->Details.AnimationData)
	//{
	//	GetMesh()->SetSkeletalMeshAsset(PlayerStateRef->Details.AnimationData->SkeletalMesh);
	//
	//	if (TemplatedAnimationBlueprint)
	//		GetMesh()->SetAnimInstanceClass(TemplatedAnimationBlueprint);
	//}
	//	
	//
	//PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Cow Colour"), PlayerStateRef->Details.CharacterColour);
	//PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Spot Colour"), PlayerStateRef->Details.CharacterSubColour);
	//
	//GetMesh()->SetMaterial(0, PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]);
}

void ALobbyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ALobbyPlayerState* MyPlayerState = GetPlayerState<ALobbyPlayerState>();
	if (!IsValid(MyPlayerState))
		return;

	MyPlayerState->OnRep_CharacterDetails();

	if (!IsValid(HUD))
		return;

	HUD->OnRep_CharacterDetails(MyPlayerState->Details);
}



