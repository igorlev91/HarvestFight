

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerNamesTeam2.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_PlayerNamesTeam2 : public UUserWidget
{
	GENERATED_BODY()

	
	/* Public Functions*/
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerNames();
	
	class APrototype2Gamestate* GameStateReference;
	
	/* Team 2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* Team2VerticalBox;

	/* Overlays */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UOverlay*> OverlaysTeam2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT2_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT2_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT2_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT2_4;

	/* Icons & rings */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UImage*> PlayerIconsTeam2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT2_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT2_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT2_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT2_4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> PlayerRingsTeam2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT2_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT2_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT2_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT2_4;

	/* Team 2 names */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UTextBlock*> PlayerNamesTeam2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT2_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT2_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT2_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT2_4;

	/* Team 2 backgrounds */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> PlayerBackgrounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT2_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT2_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT2_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT2_4;
};
