#include "PrototypeGameInstance.h"

#include "Prototype2/Characters/Prototype2Character.h"

UPrototypeGameInstance::UPrototypeGameInstance()
{
	static ConstructorHelpers::FClassFinder<UAnimInstance> FoundLobbyAnimBP(TEXT("/Game/AlphaCharacters/AnimBP_Lobby"));
	if (FoundLobbyAnimBP.Class != NULL)
	{
		LobbyAnimBP = FoundLobbyAnimBP.Class;
	}
}

void UPrototypeGameInstance::OnStart()
{
	Super::OnStart();
	for(UMaterialInstance* Material : PlayerMaterials)
	{
		PlayerMaterialsDynamic.Add(UMaterialInstanceDynamic::Create(Material,nullptr));
	}
}

void UPrototypeGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

void UPrototypeGameInstance::ResetCachedPlayerDetails()
{
	FinalPlayerDetails.Empty();
	FinalConnectionCount = 0;
	MaxPlayersOnServer = 0;
}

void UPrototypeGameInstance::ShowLoadingScreen(UUserWidget *Widget, int32 ZOrder)
{
	if (IsValid(this) && IsValid(Widget))
	{
		if (IsValid(GetGameViewportClient()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Test"));
			GetGameViewportClient()->AddViewportWidgetContent(Widget->TakeWidget());
		}
	}
}

void UPrototypeGameInstance::RemoveLoadingScreen(UUserWidget *Widget)
{
	GetGameViewportClient()->RemoveViewportWidgetContent(Widget->TakeWidget());
}
