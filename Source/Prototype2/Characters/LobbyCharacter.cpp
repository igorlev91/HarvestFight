

#include "LobbyCharacter.h"

#include "Components/TextBlock.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Prototype2/Gamemodes/LobbyGamemode.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Components/WidgetComponent.h"
#include "Prototype2/Widgets/Widget_PlayerName.h"

void ALobbyCharacter::SetPlayerStateRef(ALobbyPlayerState* _NewLobbyPlayerState)
{
	PlayerStateRef = _NewLobbyPlayerState;
}

void ALobbyCharacter::SetNameWidget(FString _name)
{
	//// do
	//if (PlayerNameWidgetComponent)
	//{
	//	if (auto Widget = PlayerNameWidgetComponent->GetWidget())
	//	{
	//		if (auto NameWidget = Cast<UWidget_PlayerName>(Widget))
	//		{
	//			NameWidget->PlayerName->SetText(FText::FromString(_name));
	//		}
	//	}
	//}
	//Multi_SetNameWidget(_name);
}

void ALobbyCharacter::SetNameWidgetPlayerRef()
{
	//if (PlayerNameWidgetComponent)
	//{
	//	if (auto Widget = PlayerNameWidgetComponent->GetWidget())
	//	{
	//		if (auto NameWidget = Cast<UWidget_PlayerName>(Widget))
	//		{
	//			NameWidget->SetPlayerRef(this);
	//		}
	//	}
	//}
}

void ALobbyCharacter::Multi_SetNameWidget_Implementation(const FString& _name)
{
	//if (PlayerNameWidgetComponent)
	//{
	//	if (auto Widget = PlayerNameWidgetComponent->GetWidget())
	//	{
	//		if (auto NameWidget = Cast<UWidget_PlayerName>(Widget))
	//		{
	//			NameWidget->PlayerName->SetText(FText::FromString(_name));
	//		}
	//	}
	//}
}

ALobbyCharacter::ALobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	///* Display name widget (above head) */
	//PlayerNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameWidgetComponent"));
	//PlayerNameWidgetComponent->SetupAttachment(RootComponent);
	//PlayerNameWidgetComponent->SetIsReplicated(false);
	//PlayerNameWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	///* Ready button widget (above head) */
	//ReadyImageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ReadyImageWidgetComponent"));
	//ReadyImageWidgetComponent->SetupAttachment(RootComponent);
	//ReadyImageWidgetComponent->SetIsReplicated(false);
	//ReadyImageWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerStateRef = GetPlayerState<ALobbyPlayerState>();
	
	//if (PlayerNameWidgetComponent)
	//{
	//	if (auto Widget = PlayerNameWidgetComponent->GetWidget())
	//	{
	//		if (auto NameWidget = Cast<UWidget_PlayerName>(Widget))
	//		{
	//			PlayerNameWidget = NameWidget;
	//		}
	//	}
	//}

	for(auto Material : PlayerMaterials)
	{
		PlayerMaterialsDynamic.Add(UMaterialInstanceDynamic::Create(Material,this));
	}

	//ReadyImageWidgetComponent->SetVisibility(false);
	
	if (IsLocallyControlled())
	{
		GetMesh()->SetVisibility(true);
	}
	else
	{
		GetMesh()->SetVisibility(false);
	}
}

void ALobbyCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (!PlayerStateRef)
	{
		PlayerStateRef = GetPlayerState<ALobbyPlayerState>();
	}
	if (IsValid(PlayerNameWidget) && IsValid(PlayerStateRef))
	{
		PlayerNameWidget->SetPlayerRef(PlayerStateRef);
	}

	if (PlayerStateRef)
	{
		///* Toggle ready image */
		//if (PlayerStateRef->IsReady)
		//{
		//	ReadyImageWidgetComponent->SetVisibility(true);
		//}
		//else
		//{
		//	ReadyImageWidgetComponent->SetVisibility(false);
		//}

		SyncCharacterSkin();
	}
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
		if (PlayerMeshes.Num() > (int)PlayerStateRef->Details.Character)
		{
			GetMesh()->SetSkeletalMeshAsset(PlayerMeshes[(int)PlayerStateRef->Details.Character]);
		}
	}
}

void ALobbyCharacter::SyncCharacterSkin()
{
	if (PlayerMeshes.Num() > (int)PlayerStateRef->Details.Character)
	{
		GetMesh()->SetSkeletalMeshAsset(PlayerMeshes[(int32)PlayerStateRef->Details.Character]);
	}
	
	PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Cow Colour"), PlayerStateRef->Details.CharacterColour);
	PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Spot Colour"), PlayerStateRef->Details.CharacterSubColour);
	
	GetMesh()->SetMaterial(0, PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]);
}



