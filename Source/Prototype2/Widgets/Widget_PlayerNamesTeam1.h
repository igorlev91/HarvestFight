

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerNamesTeam1.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_PlayerNamesTeam1 : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions*/
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/* Public Variables */
public:

	class APrototype2Gamestate* GameStateReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UVerticalBox* Team1VerticalBox;

	/* Overlays */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UOverlay*> OverlaysTeam1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT1_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT1_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT1_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* OverlayT1_4;

	/* Icons & rings */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UImage*> PlayerIconsTeam1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT1_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT1_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT1_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerIconT1_4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> PlayerRingsTeam1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT1_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT1_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT1_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* PlayerRingT1_4;

	/* Team 1 names */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class UTextBlock*> PlayerNamesTeam1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT1_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT1_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT1_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* PlayerNameT1_4;

	/* Team 1 backgrounds */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> PlayerBackgrounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT1_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT1_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT1_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* BackgroundT1_4;
	

	
	
};
