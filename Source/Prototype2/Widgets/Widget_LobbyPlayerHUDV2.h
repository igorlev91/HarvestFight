

#pragma once

#include "CoreMinimal.h"
#include "Widget_LobbyPlayerHUD.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Prototype2/PlayerStates/LobbyPlayerState.h"
#include "Widget_LobbyPlayerHUDV2.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_LobbyPlayerHUDV2 : public UWidget_LobbyPlayerHUD
{
	GENERATED_BODY()

	/* Public Functions */
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateMapChoice(UWidget_MapChoice* _MapChoiceWidget);
	void UpdateMapChoiceTimer(UWidget_MapChoice* _MapChoiceWidget);

	/* Public Variables */
public:

	UFUNCTION()
	void SetOwningController(int32 _PlayerID,class APrototype2PlayerController* _Owner);

	UFUNCTION(Client, Reliable)
	void Client_SetOwningController(int32 _PlayerID,class APrototype2PlayerController* _Owner);

	UFUNCTION()
	void InitTeams();

	UFUNCTION()
	void UpdateTeams();
	
	/* Brawl Mode Map Choice Widget*/
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoiceBrawl;

	/* Blitz Mode Map Choice Widget*/
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UWidget_MapChoice* WBP_MapChoiceBlitz;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UWidget_LobbyCharacterSelection* WBP_LobbyCharacterSelection;

	// teams
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTeams{};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget)) 
	class UTextBlock* TeamAText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget)) 
	UTextBlock* TeamBText;

	UPROPERTY(EditAnywhere)
	class UColourData* ColourData;
	
	/* Icon rings */
	UPROPERTY(EditAnywhere)
	UTexture2D* RingTexture{};

	/* Vertical Boxes - holding players overlays */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UVerticalBox* VerticalBoxLeft;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UVerticalBox* VerticalBoxRight;
	
	/* Array of all overlays */
	UPROPERTY(VisibleAnywhere)
	TArray<class UOverlay*> Overlays;
	
	/* Player Icons */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer1;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer2;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer3;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer4;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer5;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* OverlayPlayer6;
	
	/* Array of all icons */
	UPROPERTY(VisibleAnywhere)
	TArray<class UImage*> Icons;
	
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
	TArray<UImage*> Backgrounds;
	
	/* Player rings */
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P1Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P2Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P3Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P4Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P5Background;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UImage* P6Background;

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

	/* Team borders */
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* LeftTeamOverlay;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UOverlay* RightTeamOverlay;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* BackgroundImageT1;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* BackgroundImageT2;
};
