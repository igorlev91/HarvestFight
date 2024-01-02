// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyCharacter.h"

#include "LobbyPlayerState.h"
#include "Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"

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
	
}

// Called every frame
void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetMesh()->SetMaterial(0, PlayerMat);
	
	//if (HasAuthority())
	//	Multi_UpdatePlayerMaterial();
	//
	//if (auto gamestate = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld())))
	//{
	//	if (auto gameMode = Cast<ALobbyGamemode>(UGameplayStatics::GetGameMode(GetWorld())))
	//	{
	//		for(auto i = 0; i < gamestate->Server_Players.Num(); i++)
	//		{
	//			if (auto playerState = gamestate->Server_Players[i])
	//			{
	//				auto character = Cast<ALobbyCharacter>(playerState->GetPlayerController()->GetCharacter());
	//				
	//				if (character)
	//				{
	//					character->GetMesh()->SetMaterial(0, gameMode->PlayerMaterials[(int)playerState->CharacterColour]);
	//				}
	//			}
	//				
	//		}
	//	}
	//}
}

// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALobbyCharacter::Server_UpdatePlayerMaterial_Implementation()
{
	Multi_UpdatePlayerMaterial();
}

void ALobbyCharacter::Multi_UpdatePlayerMaterial_Implementation()
{
	if (auto gameMode = Cast<ALobbyGamemode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s material set: %s"), *FString::FromInt(GetPlayerState<ALobbyPlayerState>()->Player_ID), *FString::FromInt((int)GetPlayerState<ALobbyPlayerState>()->CharacterColour));
		GetMesh()->SetMaterial(0, gameMode->PlayerMaterials[(int)GetPlayerState<ALobbyPlayerState>()->CharacterColour]);
	}
}



