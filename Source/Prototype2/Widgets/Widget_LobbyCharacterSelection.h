

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Math/Vector.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Widget_LobbyCharacterSelection.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_LobbyCharacterSelection : public UUserWidget, public IWidgetUtilityInterface
{
	GENERATED_BODY()

	/* Public functions */
public:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION()
	void UpdateButtonVisibility();
	
	/* Updates the UI character image based on the selected character */
	UFUNCTION(BlueprintCallable)
	void UpdateCharacterImage();

	/**
	 * @brief Changes the character colour
	 * @param _bIsTowardsRight 
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeCharacterColour(bool _bIsTowardsRight);

	UFUNCTION(BlueprintCallable)
	bool ChangeTeamsCharacterColour(bool _bIsTowardsRight);

	/**
	 * @brief Changes the character model
	 * @param _bIsTowardsRight 
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeCharacter(bool _bIsTowardsRight);

	UFUNCTION(BlueprintCallable)
	void SetCharacterColourFromSelection();

	UFUNCTION(BlueprintCallable)
	void SetCharacterModelFromSelection();

	UFUNCTION()
	bool HasSamePlayerColour();

	UFUNCTION()
	void SetOwningController(class ALobbyPlayerController* _Owner);

	UFUNCTION()
	void SetPlayerID(int32 _PlayerID);

	UFUNCTION(BlueprintCallable)
	bool CanChangeTeams();

	TArray<EColours> GetAvailableColours();

	void UpdateWidgetSwitchers();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ALobbyGamestate* GameStateReference;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PlayerImage;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PlayerColourImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* Button_LeftColour;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* Button_RightColour;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* Button_LeftCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* Button_RightCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* Switch_CharacterLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* Switch_CharacterRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* Switch_ColorLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* Switch_ColorRight;
	

	UPROPERTY()
	bool bTeams{};

	UPROPERTY(VisibleAnywhere)
	bool bLocalReady{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bPlayerstateUpdated{};

	UPROPERTY(VisibleAnywhere)
	TArray<ESlateVisibility> PreviousColorButtonVisibilities;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterDetails IdealDetails{};

	UPROPERTY(EditAnywhere)
	int32 NumberOfCharacters{4};

	UPROPERTY(EditAnywhere)
	UColourData* SkinColourData{nullptr};

	UPROPERTY(EditAnywhere)
	class USkinData* SkinData{nullptr};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ALobbyPlayerController* OwningController{nullptr};

	/* Private Variables */
	bool HasChosenNewColour{false};
	bool HasBeenAssignedTeamColour{};

	UPROPERTY()
	int32 PlayerID{-1};
};
