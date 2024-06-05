

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Widget_MapChoice.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_MapChoice : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:

	UFUNCTION(BlueprintCallable)
	void EnableMapChoice();

	/* Sets the player map choice in the player state */
	UFUNCTION(BlueprintCallable)
	void SelectLevel(EFarm _Level);
	

	/* Public Variables */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasMapBeenSelected{false};
	
	/* Friendly Farm vote counter */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* NormalLevelCounter;

	/* Winter Farm vote counter */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* WinterLevelCounter;

	/* Honey Farm vote counter */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* HoneyLevelCounter;

	/* Honey Farm vote counter */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* FloatingIslandsLevelCounter;

	/* Honey Farm vote counter */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* ClockworkLevelCounter;

	/* Random map vote counter */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* RandomLevelCounter;

	/* Timer after players have voted on map */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* MapChoiceTimer;

	//UPROPERTY(EditAnywhere, meta=(BindWidget))
	//class UOverlay* LoadingPageFake;
};
