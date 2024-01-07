#include "LobbyCharacter.h"

#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"

UMaterialInstance* ALobbyCharacter::GetPlayerMat()
{
	return PlayerMat;
}

void ALobbyCharacter::SetPlayerMat(UMaterialInstance* _NewPlayerMat)
{
	PlayerMat = _NewPlayerMat;
}

void ALobbyCharacter::SetPlayerStateRef(ALobbyPlayerState* _NewLobbyPlayerState)
{
	PlayerStateRef = _NewLobbyPlayerState;
}

ALobbyCharacter::ALobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	GetMesh()->SetIsReplicated(true);
}

void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyCharacter, PlayerMat);
}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerStateRef = GetPlayerState<ALobbyPlayerState>();
}

void ALobbyCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!PlayerStateRef)
	{
		PlayerStateRef = GetPlayerState<ALobbyPlayerState>();
	}

	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
		Server_SetCharacterMesh();
	
	GetMesh()->SetMaterial(0, PlayerMat);
}

void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);
}

void ALobbyCharacter::Server_SetCharacterMesh_Implementation()
{
	Multi_SetCharacterMesh();
}

void ALobbyCharacter::Multi_SetCharacterMesh_Implementation()
{
	if (PlayerStateRef)
	{
		if (PlayerMeshes.Num() > (int)PlayerStateRef->Character)
		{
			GetMesh()->SetSkeletalMeshAsset(PlayerMeshes[(int)PlayerStateRef->Character]);
		}
	}
}



