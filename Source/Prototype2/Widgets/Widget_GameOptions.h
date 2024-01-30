

#pragma once

#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_GameOptions.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_GameOptions : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	/* Public Variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* GraphicsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* ConfirmButton;

	UFUNCTION()
	void OnConfirmButtonPressed();

	UFUNCTION()
	void SetOptionsText();

	UFUNCTION()
	void UpdateGameInstanceVariables();
	
	/* Child ControlWidgets */

	/* Graphics Options */
	
	// Fullscreen control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UWidget_OptionSelector* FullScreen_Control;

	UFUNCTION()
	void OnFullScreenControlLeftButtonPressed();
	UFUNCTION()
	void OnFullScreenControlRightButtonPressed();

	// Resolution control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* Resolution_Control;

	//UPROPERTY(EditAnywhere)
	//int32 ResolutionOption = (uint8)EResolution::R_1280x720;

	UPROPERTY(EditAnywhere)
	int32 ResolutionX = 1280;
	int32 ResolutionY = 720;

	UFUNCTION()
	void OnResolutionControlLeftButtonPressed();
	UFUNCTION()
	void OnResolutionControlRightButtonPressed();
	UFUNCTION()
	void SetResolutionSetting();

	// VSync control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* VSync_Control;

	UFUNCTION()
	void OnVSyncControlButtonPressed();

	// HDR Display control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* HDRDisplay_Control;

	UFUNCTION()
	void OnHDRDisplayControlButtonPressed();

	// Framerate limit control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* FramerateLimit_Control;

	UFUNCTION()
	void OnFramerateLimitControlLeftButtonPressed();
	UFUNCTION()
	void OnFramerateLimitControlRightButtonPressed();

	// Texture quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* TextureQuality_Control;

	UFUNCTION()
	void OnTextureQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnTextureQualityControlRightButtonPressed();
	UFUNCTION()
	void SetTextureQualitySetting();

	// Foliage quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* FoliageQuality_Control;

	UFUNCTION()
	void OnFoliageQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnFoliageQualityControlRightButtonPressed();
	UFUNCTION()
	void SetFoliageQualitySetting();

	// Shading quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* ShadingQuality_Control;

	UFUNCTION()
	void OnShadingQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnShadingQualityControlRightButtonPressed();
	UFUNCTION()
	void SetShadingQualitySetting();

	// Shadow quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* ShadowQuality_Control;

	UFUNCTION()
	void OnShadowQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnShadowQualityControlRightButtonPressed();
	UFUNCTION()
	void SetShadowQualitySetting();

	// Post Processing quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* PostProcessingQuality_Control;

	UFUNCTION()
	void OnPostProcessingQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnPostProcessingQualityControlRightButtonPressed();
	UFUNCTION()
	void SetPostProcessingQualitySetting();

	// Anti Aliasing quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* AntiAliasingQuality_Control;

	UFUNCTION()
	void OnAntiAliasingQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnAntiAliasingQualityControlRightButtonPressed();
	UFUNCTION()
	void SetAntiAliasingQualitySetting();

	
	/* Audio Options */

	// Master volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* MasterVolume_Control;

	UFUNCTION()
	void OnMasterVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnMasterVolumeControlRightButtonPressed();
	
	// Music volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* MusicVolume_Control;

	UFUNCTION()
	void OnMusicVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnMusicVolumeControlRightButtonPressed();

	// Ambience volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* AmbienceVolume_Control;

	UFUNCTION()
	void OnAmbienceVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnAmbienceVolumeControlRightButtonPressed();

	// Ambience volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* SFXVolume_Control;

	UFUNCTION()
	void OnSFXVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnSFXVolumeControlRightButtonPressed();

	/* Game Options */

	// Player stencil control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* PlayerStencil_Control;

	UFUNCTION()
	void OnPlayerStencilControlButtonPressed();
	
	/* Private Variables */
private:
	UPrototypeGameInstance* GameInstanceReference;

	class UHarvestHavocGameUserSettings* HHGameGameUserSettings;
};
