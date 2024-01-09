

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
};
