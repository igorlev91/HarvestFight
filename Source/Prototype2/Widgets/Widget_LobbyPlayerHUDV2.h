
#pragma once

#include "CoreMinimal.h"
#include "Widget_LobbyPlayerHUD.h"
#include "Widget_LobbyPlayerHUDV2.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_LobbyPlayerHUDV2 : public UWidget_LobbyPlayerHUD
{
	GENERATED_BODY()

	/* Public Functions */
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	
	/* Public Variables */
public:
	/* Icon rings */
	UPROPERTY(EditAnywhere)
	UTexture2D* RingTexture{};

	/* Background images for player overlays */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* LeftBoxBackgroundImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* RightBoxBackgroundImage;

	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<class UHorizontalBox*> PlayerHorizontalBoxes;
	
	/* Player Overlays */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* Player1HorizontalBox;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* Player2HorizontalBox;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* Player3HorizontalBox;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* Player4HorizontalBox;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* Player5HorizontalBox;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UHorizontalBox* Player6HorizontalBox;
	
	/* Player Icons */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Icon;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Icon;
	
	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> Rings;
	
	/* Player rings */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Ring;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Ring;

	/* Array of all rings */
	UPROPERTY(VisibleAnywhere)
	TArray<class UTextBlock*> Names;
	
	/* Players names */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	UTextBlock* P1Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	UTextBlock* P2Name; 
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	UTextBlock* P3Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	UTextBlock* P4Name;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	UTextBlock* P5Name; 
	UPROPERTY(VisibleAnywhere, meta=(BindWidget)) 
	UTextBlock* P6Name; 

	/* Array of player ready images */
	UPROPERTY(VisibleAnywhere)
	TArray<UImage*> ReadyImages;

	/* Player ready images */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5ReadyImage;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6ReadyImage;
	
};
