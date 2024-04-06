

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
/*#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"*/
#include "Widget_HostOptions.generated.h"

/*USTRUCT(BlueprintType)
struct FIPAddressResponse
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString ip;
};*/

enum EGameSpeed : int;

UCLASS()
class PROTOTYPE2_API UWidget_HostOptions : public UUserWidget, public IWidgetUtilityInterface
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnHostPressed();

	/*UFUNCTION(BlueprintCallable)
	void GetPublicIPAddress();
	void OnPublicIPAddressResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);*/

	UPROPERTY(BlueprintReadOnly)
	FString PublicIPAdress{};
	UPROPERTY(BlueprintReadOnly)
	FString PrivateIPAdress{};
	/* Public Variables */
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* IsTeamsButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* IsLanButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditableTextBox* PasswordInputField;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* StartButton;


	/* Extra Settings */
public:

	// Reset settings to defaults
	UFUNCTION(BlueprintCallable)
	void ResetDefaults();

	// Reset settings to actual setting - non temp
	UFUNCTION(BlueprintCallable)
	void ResetSetting();

	// Confirm settings 
	UFUNCTION(BlueprintCallable)
	void ConfirmSetting();
	
	// Game Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_OptionSelector* GameLength_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> DefaultGameSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> TempGameSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> GameSpeed;

	UFUNCTION()
	void OnGameSpeedControlLeftButtonPressed();
	UFUNCTION()
	void OnGameSpeedControlRightButtonPressed();
	UFUNCTION()
	void UpdateGameSpeedText();
	UFUNCTION(BlueprintCallable)
	void SetGameSpeedControl();

	// Stealing - Whether players can steal from other plots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Stealing_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultStealingSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempStealingSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool StealingSetting = true;

	UFUNCTION()
	void OnStealingControlButtonPressed();
	UFUNCTION()
	void UpdateStealingText();
	UFUNCTION(BlueprintCallable)
	void SetStealingControl();

	// Fertiliser - Whether fertiliser spawns for maps that have it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Fertiliser_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultFertiliserSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempFertiliserSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FertiliserSetting = true;

	UFUNCTION()
	void OnFertiliserControlButtonPressed();
	UFUNCTION()
	void UpdateFertiliserText();
	UFUNCTION(BlueprintCallable)
	void SetFertiliserControl();

	// Cement - Whether cement spawns for maps that have it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Cement_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultCementSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempCementSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CementSetting = true;

	UFUNCTION()
	void OnCementControlButtonPressed();
	UFUNCTION()
	void UpdateCementText();
	UFUNCTION(BlueprintCallable)
	void SetCementControl();

	// Poison - Whether poison spawns for maps that have it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Poison_Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DefaultPoisonSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TempPoisonSetting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PoisonSetting = true;

	UFUNCTION()
	void OnPoisonControlButtonPressed();
	UFUNCTION()
	void UpdatePoisonText();
	UFUNCTION(BlueprintCallable)
	void SetPoisonControl();
};
