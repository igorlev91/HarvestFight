

#include "LobbyHUD.h"

#include "Blueprint/UserWidget.h"
#include "Prototype2/Characters/LobbyCharacter.h"
#include "Prototype2/Controllers/LobbyPlayerController.h"
#include "Prototype2/Widgets/Widget_LobbyCharacterSelection.h"
#include "Prototype2/Widgets/Widget_LobbyPlayerHUDV2.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "Components/CircularThrobber.h"

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(HUDWidget_Prefab))
		return;
	
	ALobbyPlayerController* OwningController = Cast<ALobbyPlayerController>(GetOwningPlayerController());
	if (!IsValid(OwningController))
		return;

	HUDWidget = CreateWidget<UWidget_LobbyPlayerHUDV2>(OwningController, HUDWidget_Prefab);
	if (!IsValid(HUDWidget))
		return;

	HUDWidget->AddToViewport();
	HUDWidget->SetOwningController(OwningController);

	ALobbyCharacter* OwningCharacter = Cast<ALobbyCharacter>(OwningController->GetCharacter());
	if (!IsValid(OwningCharacter))
		return;

	OwningCharacter->HUD = this;
}

void ALobbyHUD::OnRep_CharacterDetails(FCharacterDetails& _SomeDetails)
{
	if(IsValid(HUDWidget) == false)
		return;

	HUDWidget->WBP_LobbyCharacterSelection->bPlayerstateUpdated = true;
	HUDWidget->WBP_LobbyCharacterSelection->IdealDetails = _SomeDetails;
	HUDWidget->WBP_LobbyCharacterSelection->UpdateCharacterImage();
}

bool ALobbyHUD::IsChangingCharacterSkin() const
{
	if(IsValid(HUDWidget) == false)
		return true;

	bool bIsChangingSkin = !HUDWidget->WBP_LobbyCharacterSelection->bPlayerstateUpdated;

	if (bIsChangingSkin)
		HUDWidget->Throbber_CharacterLoading->SetVisibility(ESlateVisibility::Visible);
	else
		HUDWidget->Throbber_CharacterLoading->SetVisibility(ESlateVisibility::Hidden);
	
	return bIsChangingSkin;
}
