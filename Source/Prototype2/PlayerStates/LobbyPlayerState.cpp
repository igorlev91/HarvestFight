#include "LobbyPlayerState.h"

#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/HUDS/LobbyHUD.h"

ALobbyPlayerState::ALobbyPlayerState()
{
}

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// Initialize HOST Skin
	if (HasAuthority() && GetPlayerController() && GetPlayerController()->IsLocalController())
	{
		UPrototypeGameInstance* SomeGameInstance = Cast<UPrototypeGameInstance>(GetGameInstance());
		if (IsValid(SomeGameInstance))
		{
			Details.AnimationData = SomeGameInstance->PlayerModels[0];
		}
		OnRep_CharacterDetails();
	}
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, IsReady);
	DOREPLIFETIME(ALobbyPlayerState, Player_ID);
	DOREPLIFETIME(ALobbyPlayerState, Details);
	DOREPLIFETIME(ALobbyPlayerState, PlayerName);
}

void ALobbyPlayerState::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
}

void ALobbyPlayerState::SetIsReady(bool _bIsReady)
{
	if (HasAuthority())
	{
		IsReady = _bIsReady;

		ALobbyGamestate* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
		if (IsValid(GameState))
		{
			GameState->CheckAllReady();
		}
	}
	else
	{
		Server_SetIsReady(_bIsReady);
	}
}

void ALobbyPlayerState::VoteMap(EFarm _Map)
{
	if (HasAuthority())
	{
		ALobbyGamestate* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
		if (IsValid(GameState))
		{
			GameState->VoteMap(_Map);
		}
	}
	else
	{
		Server_VoteMap(_Map);
	}
}

void ALobbyPlayerState::Server_VoteMap_Implementation(EFarm _Map)
{
	ALobbyGamestate* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState))
	{
		GameState->VoteMap(_Map);
	}
}

void ALobbyPlayerState::Server_SetIsReady_Implementation(bool _bIsReady)
{
	IsReady = _bIsReady;

	ALobbyGamestate* GameState = Cast<ALobbyGamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState))
	{
		GameState->CheckAllReady();
	}
}

void ALobbyPlayerState::UpdateCharacterMaterial(FCharacterDetails _Details)
{
	if (HasAuthority())
	{
		Details = _Details;
		OnRep_CharacterDetails();
	}
	else
	{
		Server_UpdateCharacterMaterial(_Details);
	}
}

void ALobbyPlayerState::Server_UpdateCharacterMaterial_Implementation(FCharacterDetails _Details)
{
	Details = _Details;
}

void ALobbyPlayerState::OnRep_CharacterDetails()
{
	AActor* SomePawn = GetPawn();
	if (!IsValid(SomePawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: LobbyPlayerState Failed To Get Pawn"));
		return;
	}
	
	ALobbyCharacter* SomeCharacter = Cast<ALobbyCharacter>(SomePawn);
	if (!IsValid(SomeCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: LobbyPlayerState Failed To Get Character"));
		return;
	}

	if (!SomeCharacter->IsLocallyControlled())
		return;

	UPrototypeGameInstance* SomeGameInstance = Cast<UPrototypeGameInstance>(GetGameInstance());
	if (!IsValid(SomeGameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: LobbyPlayerState Failed To Get GameInstance"));
		return;
	}
	
	SomeCharacter->GetMesh()->SetSkeletalMeshAsset(SomeGameInstance->LobbyPlayerModels[(int16)Details.Character]);
	
	if (IsValid(SomeGameInstance->LobbyAnimBP))
		SomeCharacter->GetMesh()->SetAnimInstanceClass(SomeGameInstance->LobbyAnimBP);
	
	SomeGameInstance->PlayerMaterialsDynamic[(int16)Details.Character]->SetVectorParameterValue(FName("Cow Colour"), Details.CharacterColour);
	SomeGameInstance->PlayerMaterialsDynamic[(int16)Details.Character]->SetVectorParameterValue(FName("Spot Colour"), Details.CharacterSubColour);
	
	SomeCharacter->GetMesh()->SetMaterial(0, SomeGameInstance->PlayerMaterialsDynamic[(int16)Details.Character]);
}
