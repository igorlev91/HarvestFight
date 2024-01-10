

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
#include "Widget_LobbyPlayerHUD.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_LobbyPlayerHUD : public UUserWidget, public IWidgetUtilityInterface
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

	/* Public Variables */
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category = "Ready Button")
	class UButton* ReadyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category = "Cancel Button")
	class UButton* CancelButton;

	class ALobbyGamestate* GameStateReference;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UWidget_MapChoice* WBP_MapChoice;

	/* Widget Variables related to button pulsing/size changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	bool bIsButtonHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	int32 ButtonIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	FVector2D ButtonScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	class UButton* ButtonToPulse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	float PulseTime;
};
