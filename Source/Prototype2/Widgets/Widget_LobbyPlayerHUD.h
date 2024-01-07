/* Bachelor of Software Engineering
 * Media Design School
 * Auckland
 * New Zealand
 * (c) Media Design School
 * File Name : Widget_LobbyPlayerHUD.h
 * Description : All functionality for player HUD in lobby map
 * Author/s : Stace Frear */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_LobbyPlayerHUD.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_LobbyPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* Updates playerstate to being ready & turns on ready UI*/
	UFUNCTION(BlueprintCallable)
	void SetReady();

	/* Updates playerstate to being not-ready & turns off ready UI */
	UFUNCTION(BlueprintCallable)
	void SetCancel();
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* ReadyButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* CancelButton;

	class ALobbyGamestate* GameStateReference;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_MapChoice* MapChoiceWidget;

	/* Player ready button (above character head) */
	// Player 1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* Player1ReadyImage;
	// Player 2
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* Player2ReadyImage;
	// Player 3
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* Player3ReadyImage;
	// Player 4
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* Player4ReadyImage;
};
