

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

UCLASS()
class PROTOTYPE2_API AGameHUD : public AHUD
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDWidget_Prefab;
	
	UPROPERTY(VisibleAnywhere)
	class UWidget_PlayerHUD* HUDWidget;
};
