


#include "GameHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(HUDWidget_Prefab))
		return;
	
	APrototype2PlayerController* OwningController = Cast<APrototype2PlayerController>(GetOwningPlayerController());
	if (!IsValid(OwningController))
		return;

	HUDWidget = CreateWidget<UWidget_PlayerHUD>(OwningController, HUDWidget_Prefab);
	if (!IsValid(HUDWidget))
		return;

	HUDWidget->AddToViewport();
	HUDWidget->SetOwningPlayer(OwningController);
	//HUDWidget->SetOwningController(OwningController);

	APrototype2Character* OwningCharacter = Cast<APrototype2Character>(OwningController->GetCharacter());
	if (!IsValid(OwningCharacter))
		return;

	OwningCharacter->PlayerHUDRef = HUDWidget;
	HUDWidget->WeaponImage->SetBrushFromTexture(OwningCharacter->CurrentWeaponSeedData->WeaponData->WeaponIcon);
}
