

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "LobbyHUD.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
public:
	void OnRep_CharacterDetails(FCharacterDetails& _SomeDetails);

	bool IsChangingCharacterSkin() const;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDWidget_Prefab;
	
	UPROPERTY(VisibleAnywhere)
	class UWidget_LobbyPlayerHUDV2* HUDWidget;

private:
	UPROPERTY(VisibleAnywhere)
	class UTexture2D* LoadingScreenBG;
};
