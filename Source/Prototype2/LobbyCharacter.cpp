// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyCharacter.h"

#include "LobbyPlayerState.h"
#include "Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/LobbyPlayerState.h"

// Sets default values
ALobbyCharacter::ALobbyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	
	GetMesh()->SetIsReplicated(true);
}

void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyCharacter, PlayerMat);
}

// Called when the game starts or when spawned
void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerStateRef = GetPlayerState<ALobbyPlayerState>();
}

// Called every frame
void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerStateRef)
	{
		PlayerStateRef = GetPlayerState<ALobbyPlayerState>();
	}

	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
		Server_SetCharacterMesh();
	
	GetMesh()->SetMaterial(0, PlayerMat);
}

void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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



