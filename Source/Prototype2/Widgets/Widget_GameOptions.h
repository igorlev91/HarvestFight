

#pragma once
#include "Prototype2/GameInstances/PrototypeGameInstance.h"

//UENUM(BlueprintType)
//enum class EWindowedMode : uint8 
//{
//	FULLSCREEN,
//	FULLSCREENWINDOW,
//	WINDOW,
//	INDEX
//};
//
//UENUM(BlueprintType)
//enum class EResolution : uint8 
//{
//	R_1280x720,
//	R_1366x768,
//	R_1600x900,
//	R_1920x1080,
//	R_2560x1080,
//	R_2560x1440,
//	R_3440x1440,
//	INDEX
//};
//
//UENUM(BlueprintType)
//enum class EToggle : uint8 
//{
//	OFF,
//	ON
//};
//
//UENUM(BlueprintType)
//enum class EFramerateLimit : uint8 
//{
//	F_UNLIMITED,
//	F_30,
//	F_60,
//	F_144,
//	INDEX
//};
//
//UENUM(BlueprintType)
//enum class EQuality : uint8 
//{
//	POTATO,
//	MEDIUM,
//	HIGH,
//	EPIC,
//	INDEX
//};

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

	//UPROPERTY(EditAnywhere)
	//int32 WindowedMode = (uint8)EWindowedMode::WINDOW;

	UFUNCTION()
	void OnFullScreenControlLeftButtonPressed();
	UFUNCTION()
	void OnFullScreenControlRightButtonPressed();
	//UFUNCTION()
	//void SetFulLScreenSetting();

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

	//UPROPERTY(EditAnywhere)
	//bool VSyncOption = (bool)EToggle::ON;

	UFUNCTION()
	void OnVSyncControlButtonPressed();
	//UFUNCTION()
	//void SetVSyncSetting();

	// HDR Display control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* HDRDisplay_Control;

	//UPROPERTY(EditAnywhere)
	//bool HDRDisplayOption = (bool)EToggle::OFF;

	UFUNCTION()
	void OnHDRDisplayControlButtonPressed();
	//UFUNCTION()
	//void SetHDRDisplaySetting();

	// Framerate limit control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* FramerateLimit_Control;

	//UPROPERTY(EditAnywhere)
	//int32 FramerateLimitOption = (uint8)EFramerateLimit::F_60;

	UFUNCTION()
	void OnFramerateLimitControlLeftButtonPressed();
	UFUNCTION()
	void OnFramerateLimitControlRightButtonPressed();
	//UFUNCTION()
	//void SetFramerateLimitSetting();

	// Texture quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* TextureQuality_Control;

	//UPROPERTY(EditAnywhere)
	//int32 TextureQualityOption = (uint8)EQuality::MEDIUM;

	UFUNCTION()
	void OnTextureQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnTextureQualityControlRightButtonPressed();
	UFUNCTION()
	void SetTextureQualitySetting();

	// Foliage quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* FoliageQuality_Control;

	//UPROPERTY(EditAnywhere)
	//int32 FoliageQualityOption = (uint8)EQuality::MEDIUM;

	UFUNCTION()
	void OnFoliageQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnFoliageQualityControlRightButtonPressed();
	UFUNCTION()
	void SetFoliageQualitySetting();

	// Shading quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* ShadingQuality_Control;

	//UPROPERTY(EditAnywhere)
	//int32 ShadingQualityOption = (uint8)EQuality::MEDIUM;

	UFUNCTION()
	void OnShadingQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnShadingQualityControlRightButtonPressed();
	UFUNCTION()
	void SetShadingQualitySetting();

	// Shadow quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* ShadowQuality_Control;

	//UPROPERTY(EditAnywhere)
	//int32 ShadowQualityOption = (uint8)EQuality::MEDIUM;

	UFUNCTION()
	void OnShadowQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnShadowQualityControlRightButtonPressed();
	UFUNCTION()
	void SetShadowQualitySetting();

	// Post Processing quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* PostProcessingQuality_Control;

	//UPROPERTY(EditAnywhere)
	//int32 PostProcessingQualityOption = (uint8)EQuality::MEDIUM;

	UFUNCTION()
	void OnPostProcessingQualityControlLeftButtonPressed();
	UFUNCTION()
	void OnPostProcessingQualityControlRightButtonPressed();
	UFUNCTION()
	void SetPostProcessingQualitySetting();

	// Anti Aliasing quality control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* AntiAliasingQuality_Control;

	//UPROPERTY(EditAnywhere)
	//int32 AntiAliasingQualityOption = (uint8)EQuality::MEDIUM;

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

	//UPROPERTY(EditAnywhere)
	//int32 MasterVolumeOption = 10;

	UFUNCTION()
	void OnMasterVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnMasterVolumeControlRightButtonPressed();
	//UFUNCTION()
	//void SetMasterVolumeSetting();
	
	// Music volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* MusicVolume_Control;

	//UPROPERTY(EditAnywhere)
	//int32 MusicVolumeOption = 5;

	UFUNCTION()
	void OnMusicVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnMusicVolumeControlRightButtonPressed();
	//UFUNCTION()
	//void SetMusicVolumeSetting();

	// Ambience volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* AmbienceVolume_Control;

	//UPROPERTY(EditAnywhere)
	//int32 AmbienceVolumeOption = 5;

	UFUNCTION()
	void OnAmbienceVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnAmbienceVolumeControlRightButtonPressed();
	//UFUNCTION()
	//void SetAmbienceVolumeSetting();

	// Ambience volume control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* SFXVolume_Control;

	//UPROPERTY(EditAnywhere)
	//int32 SFXVolumeOption = 5;

	UFUNCTION()
	void OnSFXVolumeControlLeftButtonPressed();
	UFUNCTION()
	void OnSFXVolumeControlRightButtonPressed();
	//UFUNCTION()
	//void SetSFXVolumeSetting();

	/* Game Options */

	// Player stencil control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UWidget_OptionSelector* PlayerStencil_Control;

	//UPROPERTY(EditAnywhere)
	//bool PlayerStencilOption = (bool)EToggle::OFF;

	UFUNCTION()
	void OnPlayerStencilControlButtonPressed();
	//UFUNCTION()
	//void SetPlayerStencilSetting();
	
	/* Private Variables */
private:
	//UGameUserSettings* GameUserSettings;

	UPrototypeGameInstance* GameInstanceReference;

	class UHarvestHavocGameUserSettings* HHGameGameUserSettings;
};
