

#include "Widget_GameOptions.h"

#include "Widget_OptionSelector.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "Prototype2/GameUserSettings/HarvestHavocGameUserSettings.h"

void UWidget_GameOptions::NativePreConstruct()
{
	Super::NativePreConstruct();

	/* Get world reference */
	UWorld* World = GetWorld();

	if (!World)
		return;
	
	/* Set game instance reference */
	if (auto* GameInstance = Cast<UPrototypeGameInstance>(World->GetGameInstance()))
	{
		GameInstanceReference = GameInstance;
	}
	
	HHGameGameUserSettings = UHarvestHavocGameUserSettings::GetHarvestHavocGameUserSettings();

	if (!HHGameGameUserSettings)
		UE_LOG(LogTemp, Warning, TEXT("Could not get HH game user settings"));
	
	/* Set graphics text & settings */
	FullScreen_Control->OptionText->SetText(FText::FromString("Fullscreen"));
	Resolution_Control->OptionText->SetText(FText::FromString("Resolution"));
	ResolutionScale_Control->OptionText->SetText(FText::FromString("Resolution Scale"));
	VSync_Control->OptionText->SetText(FText::FromString("VSync"));
	HDRDisplay_Control->OptionText->SetText(FText::FromString("HDR Display"));
	FramerateLimit_Control->OptionText->SetText(FText::FromString("Framerate Limit"));
	FOV_Control->OptionText->SetText(FText::FromString("Field of View"));
	MasterGraphicsQuality_Control->OptionText->SetText(FText::FromString("Master Quality Preset"));
	TextureQuality_Control->OptionText->SetText(FText::FromString("Texture Quality"));
	FoliageQuality_Control->OptionText->SetText(FText::FromString("Foliage Quality"));
	ShadingQuality_Control->OptionText->SetText(FText::FromString("Shading Quality"));
	ShadowQuality_Control->OptionText->SetText(FText::FromString("Shadow Quality"));
	PostProcessingQuality_Control->OptionText->SetText(FText::FromString("Post Processing Quality"));
	AntiAliasingQuality_Control->OptionText->SetText(FText::FromString("Anti Aliasing Quality"));
	GlobalIlluminationQuality_Control->OptionText->SetText(FText::FromString("Global Illumination Quality"));
	ReflectionQuality_Control->OptionText->SetText(FText::FromString("Reflection Quality"));
	VisualEffectQuality_Control->OptionText->SetText(FText::FromString("Visual Effect Quality"));

	/* Set audio text & settings */
	MasterVolume_Control->OptionText->SetText(FText::FromString("Master Volume"));
	MusicVolume_Control->OptionText->SetText(FText::FromString("Music Volume"));
	AmbienceVolume_Control->OptionText->SetText(FText::FromString("Ambience Volume"));
	SFXVolume_Control->OptionText->SetText(FText::FromString("SFX Volume"));
	
	/* Set Game Option settings */
	PlayerStencil_Control->OptionText->SetText(FText::FromString("Player Coloured Outline"));
	EnemyStencilAlwaysRed_Control->OptionText->SetText(FText::FromString("Enemy Outline Always Red"));

	/* UI */
	UIOffscreenIndicators_Control->OptionText->SetText(FText::FromString("Waypoint Indicators"));
	UIOffscreenIndicatorSize_Control->OptionText->SetText(FText::FromString("Waypoint Indicator Size"));

	/* Control (mouse/controller) */
	MouseSensitivityScale_Control->OptionText->SetText(FText::FromString("Mouse/Controller Sensitivity"));
	CustomMouseCursor_Control->OptionText->SetText(FText::FromString("Custom Mouse Cursor"));
	ControllerMenuSensitivityScale_Control->OptionText->SetText(FText::FromString("Controller Menu Sensitivity"));
}

void UWidget_GameOptions::NativeConstruct()
{
	Super::NativeConstruct();

	LoadSettings();
	
	/* Graphics control buttons */
	if (FullScreen_Control)
	{
		FullScreen_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFullScreenControlLeftButtonPressed);
		FullScreen_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFullScreenControlRightButtonPressed);
	}

	if (Resolution_Control)
	{
		Resolution_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnResolutionControlLeftButtonPressed);
		Resolution_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnResolutionControlRightButtonPressed);
	}

	if (ResolutionScale_Control)
	{
		ResolutionScale_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnResolutionScaleControlLeftButtonPressed);
		ResolutionScale_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnResolutionScaleControlRightButtonPressed);
	}

	if (VSync_Control)
	{
		VSync_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnVSyncControlButtonPressed);
		VSync_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnVSyncControlButtonPressed);
	}
	
	if (HDRDisplay_Control)
	{
		HDRDisplay_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnHDRDisplayControlButtonPressed);
		HDRDisplay_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnHDRDisplayControlButtonPressed);
	}

	if (FramerateLimit_Control)
	{
		FramerateLimit_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFramerateLimitControlLeftButtonPressed);
		FramerateLimit_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFramerateLimitControlRightButtonPressed);
	}

	if (FOV_Control)
	{
		FOV_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFOVControlLeftButtonPressed);
		FOV_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFOVControlRightButtonPressed);
	}

	if (MasterGraphicsQuality_Control)
	{
		MasterGraphicsQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMasterGraphicsControlLeftButtonPressed);
		MasterGraphicsQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMasterGraphicsControlRightButtonPressed);
	}
	
	if (TextureQuality_Control)
	{
		TextureQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnTextureQualityControlLeftButtonPressed);
		TextureQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnTextureQualityControlRightButtonPressed);
	}

	if (FoliageQuality_Control)
	{
		FoliageQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFoliageQualityControlLeftButtonPressed);
		FoliageQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnFoliageQualityControlRightButtonPressed);
	}

	if (ShadingQuality_Control)
	{
		ShadingQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnShadingQualityControlLeftButtonPressed);
		ShadingQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnShadingQualityControlRightButtonPressed);
	}

	if (ShadowQuality_Control)
	{
		ShadowQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnShadowQualityControlLeftButtonPressed);
		ShadowQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnShadowQualityControlRightButtonPressed);
	}

	if (PostProcessingQuality_Control)
	{
		PostProcessingQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnPostProcessingQualityControlLeftButtonPressed);
		PostProcessingQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnPostProcessingQualityControlRightButtonPressed);
	}

	if (AntiAliasingQuality_Control)
	{
		AntiAliasingQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnAntiAliasingQualityControlLeftButtonPressed);
		AntiAliasingQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnAntiAliasingQualityControlRightButtonPressed);
	}

	if (GlobalIlluminationQuality_Control)
	{
		GlobalIlluminationQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnGlobalIlluminationQualityControlLeftButtonPressed);
		GlobalIlluminationQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnGlobalIlluminationQualityControlRightButtonPressed);
	}

	if (ReflectionQuality_Control)
	{
		ReflectionQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnReflectionQualityControlLeftButtonPressed);
		ReflectionQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnReflectionQualityControlRightButtonPressed);
	}

	if (VisualEffectQuality_Control)
	{
		VisualEffectQuality_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnVisualEffectQualityControlButtonPressed);
		VisualEffectQuality_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnVisualEffectQualityControlButtonPressed);
	}

	/* Audio control buttons */
	if (MasterVolume_Control)
	{
		MasterVolume_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMasterVolumeControlLeftButtonPressed);
		MasterVolume_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMasterVolumeControlRightButtonPressed);
	}
	
	if (MusicVolume_Control)
	{
		MusicVolume_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMusicVolumeControlLeftButtonPressed);
		MusicVolume_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMusicVolumeControlRightButtonPressed);
	}

	if (AmbienceVolume_Control)
	{
		AmbienceVolume_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnAmbienceVolumeControlLeftButtonPressed);
		AmbienceVolume_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnAmbienceVolumeControlRightButtonPressed);
	}

	if (SFXVolume_Control)
	{
		SFXVolume_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnSFXVolumeControlLeftButtonPressed);
		SFXVolume_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnSFXVolumeControlRightButtonPressed);
	}
	
	/* Game setting control buttons */
	if (PlayerStencil_Control)
	{
		PlayerStencil_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnPlayerStencilControlButtonPressed);
		PlayerStencil_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnPlayerStencilControlButtonPressed);
	}

	if (EnemyStencilAlwaysRed_Control)
	{
		EnemyStencilAlwaysRed_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnEnemyStencilAlwaysRedControlButtonPressed);
		EnemyStencilAlwaysRed_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnEnemyStencilAlwaysRedControlButtonPressed);
	}

	/* UI */
	if (UIOffscreenIndicators_Control)
	{
		UIOffscreenIndicators_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnUIOffscreenIndicatorsControlLeftButtonPressed);
		UIOffscreenIndicators_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnUIOffscreenIndicatorsControlRightButtonPressed);
	}

	if (UIOffscreenIndicatorSize_Control)
	{
		UIOffscreenIndicatorSize_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnUIOffscreenIndicatorSizeControlButtonPressed);
		UIOffscreenIndicatorSize_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnUIOffscreenIndicatorSizeControlButtonPressed);
	}

	/* Control (mouse/controller sensitivity) */
	if (MouseSensitivityScale_Control)
	{
		MouseSensitivityScale_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMouseSensitivityScaleControlLeftButtonPressed);
		MouseSensitivityScale_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnMouseSensitivityScaleControlRightButtonPressed);
	}
	if (CustomMouseCursor_Control)
	{
		CustomMouseCursor_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnCustomMouseCursorControlButtonPressed);
		CustomMouseCursor_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnCustomMouseCursorControlButtonPressed);
	}
	if (ControllerMenuSensitivityScale_Control)
	{
		ControllerMenuSensitivityScale_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnControllerMenuSensitivityScaleControlLeftButtonPressed);
		ControllerMenuSensitivityScale_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_GameOptions::OnControllerMenuSensitivityScaleControlRightButtonPressed);
	}

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UWidget_GameOptions::OnConfirmButtonPressed);
	}
}

void UWidget_GameOptions::OnConfirmButtonPressed()
{
	/* Apply settings */
	if (!HHGameGameUserSettings)
		return;

	HHGameGameUserSettings->FieldOfView = TempFOV;
	HHGameGameUserSettings->MasterGraphics = TempMasterGraphics;
	HHGameGameUserSettings->bPlayerStencil = bTempPlayerStencil;
	HHGameGameUserSettings->bEnemyAlwaysRed = bTempEnemyAlwaysRed;
	HHGameGameUserSettings->UIIndicators = TempUIIndicators;
	HHGameGameUserSettings->UIIndicatorSizeLarge = bTempUIIndicatorSizeLarge;
	HHGameGameUserSettings->MouseSensitivityScale = TempMouseSensitivityScale;
	HHGameGameUserSettings->CustomMouseCursor = TempCustomMouseCursor;
	HHGameGameUserSettings->ControllerMenuSensitivityScale = TempControllerMenuSensitivityScale;
	OnFOVChangedDelegate.Broadcast();
	HHGameGameUserSettings->ApplySettings(true);

	UpdateGameInstanceVariables();
}

void UWidget_GameOptions::SetOptionsText()
{
	if (!HHGameGameUserSettings)
		return;

	/* Fullscreen mode */
	switch (HHGameGameUserSettings->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Fullscreen"));
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Windowed Fullscreen"));
			break;
		}
	case EWindowMode::Windowed:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Windowed"));
			break;
		}
	default:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Default Fullscreen"));
			break;
		}
	}

	/* Resolution */
	const FIntPoint Resolution = HHGameGameUserSettings->GetScreenResolution();

	if (Resolution.X == 1280 && Resolution.Y == 720)
		Resolution_Control->OptionValueText->SetText(FText::FromString("1280x720"));
	else if (Resolution.X == 1366 && Resolution.Y == 768)
		Resolution_Control->OptionValueText->SetText(FText::FromString("1366x768"));
	else if (Resolution.X == 1600 && Resolution.Y == 900)
		Resolution_Control->OptionValueText->SetText(FText::FromString("1600x900"));
	else if (Resolution.X == 1920 && Resolution.Y == 1080)
		Resolution_Control->OptionValueText->SetText(FText::FromString("1920x1080"));
	else if (Resolution.X == 2560 && Resolution.Y == 1080)
		Resolution_Control->OptionValueText->SetText(FText::FromString("2560x1080"));
	else if (Resolution.X == 2560 && Resolution.Y == 1440)
		Resolution_Control->OptionValueText->SetText(FText::FromString("2560x1440"));
	else if (Resolution.X == 3440 && Resolution.Y == 1440)
		Resolution_Control->OptionValueText->SetText(FText::FromString("3440x1440"));

	/* Resolution scale */
	SetResolutionScaleSetting();

	/* VSync */
	if (HHGameGameUserSettings->IsVSyncEnabled() == true)
	{
		VSync_Control->OptionValueText->SetText(FText::FromString("On"));
	}
	else
	{
		VSync_Control->OptionValueText->SetText(FText::FromString("Off"));
	}

	/* HDR */
	if (HHGameGameUserSettings->IsHDREnabled() == true)
	{
		HDRDisplay_Control->OptionValueText->SetText(FText::FromString("On"));
	}
	else
	{
		HDRDisplay_Control->OptionValueText->SetText(FText::FromString("Off"));
	}

	/* Framerate Limit */
	switch ((int32)HHGameGameUserSettings->GetFrameRateLimit())
	{
	case 0:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("Unlimited"));
			break;
		}
	case 30:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("30"));
			break;
		}
	case 60:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("60"));
			break;
		}
	case 144:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("144"));
			break;
		}
	default:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("Default 60"));
			break;
		}
	}
	SetFOVQualitySettingText();
	SetMasterGraphicsQualitySettingText();
	SetTextureQualitySetting();
	SetFoliageQualitySetting();
	SetShadingQualitySetting();
	SetShadowQualitySetting();
	SetPostProcessingQualitySetting();
	SetAntiAliasingQualitySetting();
	SetGlobalIlluminationQualitySetting();
	SetReflectionQualitySetting();
	SetVisualEffectQualitySetting();

	MasterVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMasterVolume())));
	MusicVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMusicVolume())));
	AmbienceVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetAmbienceVolume())));
	SFXVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetSFXVolume())));

	/* Player Outline */
	if (HHGameGameUserSettings->GetPlayerStencil() == true)
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("On"));
	}
	else
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("Off"));
	}

	if (HHGameGameUserSettings->bEnemyAlwaysRed == true)
	{
		EnemyStencilAlwaysRed_Control->OptionValueText->SetText(FText::FromString("On"));
	}
	else
	{
		EnemyStencilAlwaysRed_Control->OptionValueText->SetText(FText::FromString("Off"));
	}

	SetUIOffscreenIndicatorsSetting();
	SetUIOffscreenIndicatorSizeSettingText();
	SetMouseSensitivityScaleSettingText();
	SetCustomMouseCursorSettingText();
	SetControllerMenuSensitivityScaleSettingText();
}

void UWidget_GameOptions::UpdateGameInstanceVariables()
{
	if (!GameInstanceReference)
		return;

	/* Game Options */
	GameInstanceReference->bPlayerStentil = HHGameGameUserSettings->GetPlayerStencil();
}

void UWidget_GameOptions::LoadSettings()
{
	/* Load settings */
	if (HHGameGameUserSettings)
	{
		HHGameGameUserSettings->LoadSettings(true);
		OnLoadAudioSettings(	(float)HHGameGameUserSettings->GetMasterVolume()/10,
								(float)HHGameGameUserSettings->GetAmbienceVolume()/10,
								(float)HHGameGameUserSettings->GetSFXVolume()/10,
								(float)HHGameGameUserSettings->GetMusicVolume()/10);
		
		//UE_LOG(LogTemp, Warning, TEXT("Loaded Master Volume: %d"), HHGameGameUserSettings->GetMasterVolume());
		//UE_LOG(LogTemp, Warning, TEXT("Loaded Music Volume: %d"), HHGameGameUserSettings->GetMusicVolume());
		//UE_LOG(LogTemp, Warning, TEXT("Loaded Ambience Volume: %d"), HHGameGameUserSettings->GetAmbienceVolume());
		//UE_LOG(LogTemp, Warning, TEXT("Loaded SFX Volume: %d"), HHGameGameUserSettings->GetSFXVolume());

		//UE_LOG(LogTemp, Warning, TEXT("Loaded Framerate: %f"), HHGameGameUserSettings->GetFrameRateLimit());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load settings"));
	}
	bTempPlayerStencil = HHGameGameUserSettings->bPlayerStencil;
	bTempEnemyAlwaysRed = HHGameGameUserSettings->bEnemyAlwaysRed;
	TempUIIndicators = HHGameGameUserSettings->UIIndicators;
	bTempUIIndicatorSizeLarge = HHGameGameUserSettings->UIIndicatorSizeLarge;
	TempMouseSensitivityScale = HHGameGameUserSettings->MouseSensitivityScale;
	TempMasterGraphics = HHGameGameUserSettings->MasterGraphics;
	TempFOV = HHGameGameUserSettings->FieldOfView;
	TempControllerMenuSensitivityScale = HHGameGameUserSettings->ControllerMenuSensitivityScale;
	TempCustomMouseCursor = HHGameGameUserSettings->CustomMouseCursor;
	UpdateGameInstanceVariables();
	SetOptionsText();
	
}

void UWidget_GameOptions::OnFullScreenControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	switch (HHGameGameUserSettings->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Windowed Fullscreen"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Windowed"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
			break;
		}
	case EWindowMode::Windowed:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Fullscreen"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;
		}
	default:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Default Fullscreen"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;
		}
	}
}

void UWidget_GameOptions::OnFullScreenControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	switch (HHGameGameUserSettings->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Windowed"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Fullscreen"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);

			const FIntPoint Resolution = HHGameGameUserSettings->GetDesktopResolution();
			HHGameGameUserSettings->SetScreenResolution(Resolution);
			break;
		}
	case EWindowMode::Windowed:
		{
			FullScreen_Control->OptionValueText->SetText(FText::FromString("Windowed Fullscreen"));
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
			break;
		}
	default:
		{
			HHGameGameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
			break;
		}
	}
}

void UWidget_GameOptions::OnResolutionControlLeftButtonPressed()
{
	if (ResolutionOption >= 0)
		ResolutionOption -= 1;
	if (ResolutionOption < 0)
		ResolutionOption = (uint8)EResolution::INDEX - 1;

	SetResolutionSetting();
}

void UWidget_GameOptions::OnResolutionControlRightButtonPressed()
{
	if (ResolutionOption < (uint8)EResolution::INDEX)
		ResolutionOption += 1;
	if (ResolutionOption >= (uint8)EResolution::INDEX)
		ResolutionOption = 0;

	SetResolutionSetting();
}

void UWidget_GameOptions::SetResolutionSetting()
{
	if (!HHGameGameUserSettings)
		return;
	
	switch (ResolutionOption)
	{
	case (uint8)EResolution::R_1280x720:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("1280x720"));
			ResolutionX = 1280;
			ResolutionY = 720;
			break;
		}
	case (uint8)EResolution::R_1366x768:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("1366x768"));
			ResolutionX = 1366;
			ResolutionY = 768;
			break;
		}
	case (uint8)EResolution::R_1600x900:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("1600x900"));
			ResolutionX = 1600;
			ResolutionY = 900;
			break;
		}
	case (uint8)EResolution::R_1920x1080:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("1920x1080"));
			ResolutionX = 1920;
			ResolutionY = 1080;
			break;
		}
	case (uint8)EResolution::R_2560x1440:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("2560x1440"));
			ResolutionX = 2560;
			ResolutionY = 1440;
			break;
		}
	case (uint8)EResolution::R_2560x1080:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("2560x1080"));
			ResolutionX = 2560;
			ResolutionY = 1080;
			break;
		}
	case (uint8)EResolution::R_3440x1440:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("3440x1440"));
			ResolutionX = 3440;
			ResolutionY = 1440;
			break;
		}
	default:
		{
			Resolution_Control->OptionValueText->SetText(FText::FromString("Default 1280x720"));
			ResolutionX = 1280;
			ResolutionY = 720;
			break;
		}
	}
	
	// Set the desired screen resolution (optional)
	const FIntPoint Resolution = FIntPoint(ResolutionX, ResolutionY); // Change this to your desired resolution
	HHGameGameUserSettings->SetScreenResolution(Resolution);
}

void UWidget_GameOptions::OnResolutionScaleControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.9f)
		HHGameGameUserSettings->SetResolutionScaleNormalized(0.75f);
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.7f)
		HHGameGameUserSettings->SetResolutionScaleNormalized(0.5f);
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.4f)
		HHGameGameUserSettings->SetResolutionScaleNormalized(0.25f);
	else
		HHGameGameUserSettings->SetResolutionScaleNormalized(1.0f);

	SetResolutionScaleSetting();
}

void UWidget_GameOptions::OnResolutionScaleControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.9f)
		HHGameGameUserSettings->SetResolutionScaleNormalized(0.25f);
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.7f)
		HHGameGameUserSettings->SetResolutionScaleNormalized(1.0f);
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.4f)
		HHGameGameUserSettings->SetResolutionScaleNormalized(0.75f);
	else
		HHGameGameUserSettings->SetResolutionScaleNormalized(0.5f);

	SetResolutionScaleSetting();
}

void UWidget_GameOptions::SetResolutionScaleSetting()
{
	if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.9f)
		ResolutionScale_Control->OptionValueText->SetText(FText::FromString("100"));
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.7f)
		ResolutionScale_Control->OptionValueText->SetText(FText::FromString("75"));
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.4f)
		ResolutionScale_Control->OptionValueText->SetText(FText::FromString("50"));
	else if (HHGameGameUserSettings->GetResolutionScaleNormalized() >= 0.2f)
		ResolutionScale_Control->OptionValueText->SetText(FText::FromString("25"));

	UE_LOG(LogTemp, Warning, TEXT("Resolution Scale: %f"), HHGameGameUserSettings->GetResolutionScaleNormalized());
}

void UWidget_GameOptions::OnVSyncControlButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->IsVSyncEnabled() == true)
	{
		VSync_Control->OptionValueText->SetText(FText::FromString("Off"));
		HHGameGameUserSettings->SetVSyncEnabled(false);
	}
	else
	{
		VSync_Control->OptionValueText->SetText(FText::FromString("On"));
		HHGameGameUserSettings->SetVSyncEnabled(true);
	}
}

void UWidget_GameOptions::OnHDRDisplayControlButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->IsHDREnabled() == true)
	{
		VSync_Control->OptionValueText->SetText(FText::FromString("Off"));
		HHGameGameUserSettings->EnableHDRDisplayOutput(false);
	}
	else
	{
		VSync_Control->OptionValueText->SetText(FText::FromString("On"));
		HHGameGameUserSettings->EnableHDRDisplayOutput(true);
	}
}

void UWidget_GameOptions::OnFramerateLimitControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	switch ((int32)HHGameGameUserSettings->GetFrameRateLimit())
	{
	case 0:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("144"));
			HHGameGameUserSettings->SetFrameRateLimit(144);
			break;
		}
	case 30:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("Unlimited"));
			HHGameGameUserSettings->SetFrameRateLimit(0);
			break;
		}
	case 60:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("30"));
			HHGameGameUserSettings->SetFrameRateLimit(30);
			break;
		}
	case 144:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("60"));
			HHGameGameUserSettings->SetFrameRateLimit(60);
			break;
		}
	default:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("60"));
			HHGameGameUserSettings->SetFrameRateLimit(60);
			break;
		}
	}
}

void UWidget_GameOptions::OnFramerateLimitControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	switch ((int32)HHGameGameUserSettings->GetFrameRateLimit())
	{
	case 0:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("30"));
			HHGameGameUserSettings->SetFrameRateLimit(30);
			break;
		}
	case 30:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("60"));
			HHGameGameUserSettings->SetFrameRateLimit(60);
			break;
		}
	case 60:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("144"));
			HHGameGameUserSettings->SetFrameRateLimit(144);
			break;
		}
	case 144:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("Unlimited"));
			HHGameGameUserSettings->SetFrameRateLimit(0);
			break;
		}
	default:
		{
			FramerateLimit_Control->OptionValueText->SetText(FText::FromString("60"));
			HHGameGameUserSettings->SetFrameRateLimit(60);
			break;
		}
	}
}

void UWidget_GameOptions::OnFOVControlLeftButtonPressed()
{
	if (TempFOV > 90)
		TempFOV -= 5;
	else if (TempFOV <= 90)
		TempFOV = 120;

	SetFOVQualitySettingText();
		
	//OnFOVChangedDelegate.Broadcast();
}

void UWidget_GameOptions::OnFOVControlRightButtonPressed()
{
	if (TempFOV <= 120)
		TempFOV += 5;
	if (TempFOV > 120)
		TempFOV = 90;

	SetFOVQualitySettingText();
	
	//OnFOVChangedDelegate.Broadcast();
}

void UWidget_GameOptions::SetFOVQualitySettingText()
{
	FOV_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(TempFOV)));
}

void UWidget_GameOptions::OnMasterGraphicsControlLeftButtonPressed()
{
	if (TempMasterGraphics > 0)
		TempMasterGraphics -= 1;
	else if (TempMasterGraphics == 0)
		TempMasterGraphics = 3;

	SetMasterGraphicsQualitySettingText();
}

void UWidget_GameOptions::OnMasterGraphicsControlRightButtonPressed()
{
	if (TempMasterGraphics < 4)
		TempMasterGraphics += 1;
	if (TempMasterGraphics >= 4)
		TempMasterGraphics = 0;

	SetMasterGraphicsQualitySettingText();
}

void UWidget_GameOptions::SetMasterGraphicsQualitySettingText()
{
	switch (TempMasterGraphics)
	{
	case 0:
		{
			MasterGraphicsQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			MasterGraphicsQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			MasterGraphicsQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			MasterGraphicsQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	case 4:
		{
			MasterGraphicsQuality_Control->OptionValueText->SetText(FText::FromString("Custom"));
			break;
		}
	default:
		{
			MasterGraphicsQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}

	MatchGraphicsQualitySettings();
}

void UWidget_GameOptions::MatchGraphicsQualitySettings()
{
	if (TempMasterGraphics != 4)
	{
		HHGameGameUserSettings->SetTextureQuality(TempMasterGraphics);
		SetTextureQualitySetting();
		HHGameGameUserSettings->SetFoliageQuality(TempMasterGraphics);
		SetFoliageQualitySetting();
		HHGameGameUserSettings->SetShadingQuality(TempMasterGraphics);
		SetShadingQualitySetting();
		HHGameGameUserSettings->SetShadowQuality(TempMasterGraphics);
		SetShadowQualitySetting();
		HHGameGameUserSettings->SetPostProcessingQuality(TempMasterGraphics);
		SetPostProcessingQualitySetting();
		HHGameGameUserSettings->SetAntiAliasingQuality(TempMasterGraphics);
		SetAntiAliasingQualitySetting();
		HHGameGameUserSettings->SetGlobalIlluminationQuality(TempMasterGraphics);
		SetGlobalIlluminationQualitySetting();
		HHGameGameUserSettings->SetReflectionQuality(TempMasterGraphics);
		SetReflectionQualitySetting();

		if (TempMasterGraphics == 3)	
			HHGameGameUserSettings->SetVisualEffectQuality(3);
		else
			HHGameGameUserSettings->SetVisualEffectQuality(2);
	
		SetVisualEffectQualitySetting();
	}
}

void UWidget_GameOptions::OnTextureQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetTextureQuality() > 0)
		HHGameGameUserSettings->SetTextureQuality(HHGameGameUserSettings->GetTextureQuality() - 1);
	else if (HHGameGameUserSettings->GetTextureQuality() == 0)
		HHGameGameUserSettings->SetTextureQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetTextureQualitySetting();
}

void UWidget_GameOptions::OnTextureQualityControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetTextureQuality() < 4)
		HHGameGameUserSettings->SetTextureQuality(HHGameGameUserSettings->GetTextureQuality() + 1);
	if (HHGameGameUserSettings->GetTextureQuality() >= 4)
		HHGameGameUserSettings->SetTextureQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetTextureQualitySetting();
}

void UWidget_GameOptions::SetTextureQualitySetting()
{
	SetQualityLevelText(TextureQuality_Control, HHGameGameUserSettings->GetTextureQuality());
}

void UWidget_GameOptions::OnFoliageQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetFoliageQuality() > 0)
		HHGameGameUserSettings->SetFoliageQuality(HHGameGameUserSettings->GetFoliageQuality() - 1);
	else if (HHGameGameUserSettings->GetFoliageQuality() == 0)
		HHGameGameUserSettings->SetFoliageQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetFoliageQualitySetting();
}

void UWidget_GameOptions::OnFoliageQualityControlRightButtonPressed()
{
	if (HHGameGameUserSettings->GetFoliageQuality() < 4)
		HHGameGameUserSettings->SetFoliageQuality(HHGameGameUserSettings->GetFoliageQuality() + 1);
	if (HHGameGameUserSettings->GetFoliageQuality() >= 4)
		HHGameGameUserSettings->SetFoliageQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetFoliageQualitySetting();
}

void UWidget_GameOptions::SetFoliageQualitySetting()
{
	SetQualityLevelText(FoliageQuality_Control, HHGameGameUserSettings->GetFoliageQuality());
}

void UWidget_GameOptions::OnShadingQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetShadingQuality() > 0)
		HHGameGameUserSettings->SetShadingQuality(HHGameGameUserSettings->GetShadingQuality() - 1);
	else if (HHGameGameUserSettings->GetShadingQuality() == 0)
		HHGameGameUserSettings->SetShadingQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetShadingQualitySetting();
}

void UWidget_GameOptions::OnShadingQualityControlRightButtonPressed()
{
	if (HHGameGameUserSettings->GetShadingQuality() < 4)
		HHGameGameUserSettings->SetShadingQuality(HHGameGameUserSettings->GetShadingQuality() + 1);
	if (HHGameGameUserSettings->GetShadingQuality() >= 4)
		HHGameGameUserSettings->SetShadingQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetShadingQualitySetting();
}

void UWidget_GameOptions::SetShadingQualitySetting()
{
	SetQualityLevelText(ShadingQuality_Control, HHGameGameUserSettings->GetShadingQuality());
}

void UWidget_GameOptions::OnShadowQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetShadowQuality() > 0)
		HHGameGameUserSettings->SetShadowQuality(HHGameGameUserSettings->GetShadowQuality() - 1);
	else if (HHGameGameUserSettings->GetShadowQuality() == 0)
		HHGameGameUserSettings->SetShadowQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetShadowQualitySetting();
}

void UWidget_GameOptions::OnShadowQualityControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetShadowQuality() < 4)
		HHGameGameUserSettings->SetShadowQuality(HHGameGameUserSettings->GetShadowQuality() + 1);
	if (HHGameGameUserSettings->GetShadowQuality() >= 4)
		HHGameGameUserSettings->SetShadowQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetShadowQualitySetting();
}

void UWidget_GameOptions::SetShadowQualitySetting()
{
	SetQualityLevelText(ShadowQuality_Control, HHGameGameUserSettings->GetShadowQuality());
}

void UWidget_GameOptions::OnPostProcessingQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetPostProcessingQuality() > 0)
		HHGameGameUserSettings->SetPostProcessingQuality(HHGameGameUserSettings->GetPostProcessingQuality() - 1);
	else if (HHGameGameUserSettings->GetPostProcessingQuality() == 0)
		HHGameGameUserSettings->SetPostProcessingQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetPostProcessingQualitySetting();
}

void UWidget_GameOptions::OnPostProcessingQualityControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetPostProcessingQuality() < 4)
		HHGameGameUserSettings->SetPostProcessingQuality(HHGameGameUserSettings->GetPostProcessingQuality() + 1);
	if (HHGameGameUserSettings->GetPostProcessingQuality() >= 4)
		HHGameGameUserSettings->SetPostProcessingQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetPostProcessingQualitySetting();
}

void UWidget_GameOptions::SetPostProcessingQualitySetting()
{
	SetQualityLevelText(PostProcessingQuality_Control, HHGameGameUserSettings->GetPostProcessingQuality());
}

void UWidget_GameOptions::OnAntiAliasingQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetAntiAliasingQuality() > 0)
		HHGameGameUserSettings->SetAntiAliasingQuality(HHGameGameUserSettings->GetAntiAliasingQuality() - 1);
	else if (HHGameGameUserSettings->GetAntiAliasingQuality() == 0)
		HHGameGameUserSettings->SetAntiAliasingQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetAntiAliasingQualitySetting();
}

void UWidget_GameOptions::OnAntiAliasingQualityControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetAntiAliasingQuality() < 4)
		HHGameGameUserSettings->SetAntiAliasingQuality(HHGameGameUserSettings->GetAntiAliasingQuality() + 1);
	if (HHGameGameUserSettings->GetAntiAliasingQuality() >= 4)
		HHGameGameUserSettings->SetAntiAliasingQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetAntiAliasingQualitySetting();
}

void UWidget_GameOptions::SetAntiAliasingQualitySetting()
{
	SetQualityLevelText(AntiAliasingQuality_Control, HHGameGameUserSettings->GetAntiAliasingQuality());
}

void UWidget_GameOptions::OnGlobalIlluminationQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetGlobalIlluminationQuality() > 0)
		HHGameGameUserSettings->SetGlobalIlluminationQuality(HHGameGameUserSettings->GetGlobalIlluminationQuality() - 1);
	else if (HHGameGameUserSettings->GetGlobalIlluminationQuality() == 0)
		HHGameGameUserSettings->SetGlobalIlluminationQuality(3);
		
	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetGlobalIlluminationQualitySetting();
}

void UWidget_GameOptions::OnGlobalIlluminationQualityControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetGlobalIlluminationQuality() < 4)
		HHGameGameUserSettings->SetGlobalIlluminationQuality(HHGameGameUserSettings->GetGlobalIlluminationQuality() + 1);
	if (HHGameGameUserSettings->GetGlobalIlluminationQuality() >= 4)
		HHGameGameUserSettings->SetGlobalIlluminationQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetGlobalIlluminationQualitySetting();
}

void UWidget_GameOptions::SetGlobalIlluminationQualitySetting()
{
	SetQualityLevelText(GlobalIlluminationQuality_Control, HHGameGameUserSettings->GetGlobalIlluminationQuality());
}

void UWidget_GameOptions::OnReflectionQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetReflectionQuality() > 0)
		HHGameGameUserSettings->SetReflectionQuality(HHGameGameUserSettings->GetReflectionQuality() - 1);
	else if (HHGameGameUserSettings->GetReflectionQuality() == 0)
		HHGameGameUserSettings->SetReflectionQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetReflectionQualitySetting();
}

void UWidget_GameOptions::OnReflectionQualityControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetReflectionQuality() < 4)
		HHGameGameUserSettings->SetReflectionQuality(HHGameGameUserSettings->GetReflectionQuality() + 1);
	if (HHGameGameUserSettings->GetReflectionQuality() >= 4)
		HHGameGameUserSettings->SetReflectionQuality(0);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetReflectionQualitySetting();
}

void UWidget_GameOptions::SetReflectionQualitySetting()
{
	SetQualityLevelText(ReflectionQuality_Control, HHGameGameUserSettings->GetReflectionQuality());
}

void UWidget_GameOptions::OnVisualEffectQualityControlButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetVisualEffectQuality() == 3)
		HHGameGameUserSettings->SetVisualEffectQuality(2);
	else
		HHGameGameUserSettings->SetVisualEffectQuality(3);

	TempMasterGraphics = 4;
	SetMasterGraphicsQualitySettingText();

	SetVisualEffectQualitySetting();
}

void UWidget_GameOptions::SetVisualEffectQualitySetting()
{
	SetQualityLevelText(VisualEffectQuality_Control, HHGameGameUserSettings->GetVisualEffectQuality());
}

void UWidget_GameOptions::OnMasterVolumeControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetMasterVolume() >= 0)
		HHGameGameUserSettings->SetMasterVolume(HHGameGameUserSettings->GetMasterVolume() - 1);
	if (HHGameGameUserSettings->GetMasterVolume() < 0)
		HHGameGameUserSettings->SetMasterVolume(10);

	MasterVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMasterVolume())));
	float MasterVolume = (float)HHGameGameUserSettings->GetMasterVolume()/10;
	float AmbienceVolume = (float)HHGameGameUserSettings->GetAmbienceVolume()/10;
	float SFXVolume = (float)HHGameGameUserSettings->GetSFXVolume()/10;
	float MusicVolume = (float)HHGameGameUserSettings->GetMusicVolume()/10;
	OnMasterVolumeChanged(MasterVolume, AmbienceVolume, SFXVolume, MusicVolume);
}

void UWidget_GameOptions::OnMasterVolumeControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetMasterVolume() < 11)
		HHGameGameUserSettings->SetMasterVolume(HHGameGameUserSettings->GetMasterVolume() + 1);
	if (HHGameGameUserSettings->GetMasterVolume() >= 11)
		HHGameGameUserSettings->SetMasterVolume(0);

	MasterVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMasterVolume())));
	float MasterVolume = (float)HHGameGameUserSettings->GetMasterVolume()/10;
	float AmbienceVolume = (float)HHGameGameUserSettings->GetAmbienceVolume()/10;
	float SFXVolume = (float)HHGameGameUserSettings->GetSFXVolume()/10;
	float MusicVolume = (float)HHGameGameUserSettings->GetMusicVolume()/10;
	OnMasterVolumeChanged(MasterVolume, AmbienceVolume, SFXVolume, MusicVolume);
}

void UWidget_GameOptions::OnMusicVolumeControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetMusicVolume() >= 0)
		HHGameGameUserSettings->SetMusicVolume(HHGameGameUserSettings->GetMusicVolume() - 1);
	if (HHGameGameUserSettings->GetMusicVolume() < 0)
		HHGameGameUserSettings->SetMusicVolume(10);

	MusicVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMusicVolume())));
	float Volume = (float)HHGameGameUserSettings->GetMusicVolume()/10;
	OnMusicVolumeChanged(Volume);
}

void UWidget_GameOptions::OnMusicVolumeControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetMusicVolume() < 11)
		HHGameGameUserSettings->SetMusicVolume(HHGameGameUserSettings->GetMusicVolume() + 1);
	if (HHGameGameUserSettings->GetMusicVolume() >= 11)
		HHGameGameUserSettings->SetMusicVolume(0);

	MusicVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMusicVolume())));
	float Volume = (float)HHGameGameUserSettings->GetMusicVolume()/10;
	OnMusicVolumeChanged(Volume);
}

void UWidget_GameOptions::OnAmbienceVolumeControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetAmbienceVolume() >= 0)
		HHGameGameUserSettings->SetAmbienceVolume(HHGameGameUserSettings->GetAmbienceVolume() - 1);
	if (HHGameGameUserSettings->GetAmbienceVolume() < 0)
		HHGameGameUserSettings->SetAmbienceVolume(10);

	AmbienceVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetAmbienceVolume())));
	float Volume = (float)HHGameGameUserSettings->GetAmbienceVolume()/10;
	OnAmbienceVolumeChanged(Volume);
}

void UWidget_GameOptions::OnAmbienceVolumeControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetAmbienceVolume() < 11)
		HHGameGameUserSettings->SetAmbienceVolume(HHGameGameUserSettings->GetAmbienceVolume() + 1);
	if (HHGameGameUserSettings->GetAmbienceVolume() >= 11)
		HHGameGameUserSettings->SetAmbienceVolume(0);

	AmbienceVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetAmbienceVolume())));
	float Volume = (float)HHGameGameUserSettings->GetAmbienceVolume()/10;
	OnAmbienceVolumeChanged(Volume);
}

void UWidget_GameOptions::OnSFXVolumeControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetSFXVolume() >= 0)
		HHGameGameUserSettings->SetSFXVolume(HHGameGameUserSettings->GetSFXVolume() - 1);
	if (HHGameGameUserSettings->GetSFXVolume() < 0)
		HHGameGameUserSettings->SetSFXVolume(10);

	SFXVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetSFXVolume())));
	float Volume = (float)HHGameGameUserSettings->GetSFXVolume()/10;
	OnSFXVolumeChanged(Volume);
}

void UWidget_GameOptions::OnSFXVolumeControlRightButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetSFXVolume() < 11)
		HHGameGameUserSettings->SetSFXVolume(HHGameGameUserSettings->GetSFXVolume() + 1);
	if (HHGameGameUserSettings->GetSFXVolume() >= 11)
		HHGameGameUserSettings->SetSFXVolume(0);
	
	SFXVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetSFXVolume())));
	float Volume = (float)HHGameGameUserSettings->GetSFXVolume()/10;
	OnSFXVolumeChanged(Volume);
}

void UWidget_GameOptions::OnPlayerStencilControlButtonPressed()
{
	if (bTempPlayerStencil == true)
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("Off"));
		bTempPlayerStencil = false;
	}
	else
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("On"));
		bTempPlayerStencil = true;
	}
}

void UWidget_GameOptions::OnEnemyStencilAlwaysRedControlButtonPressed()
{
	if (bTempEnemyAlwaysRed == true)
	{
		EnemyStencilAlwaysRed_Control->OptionValueText->SetText(FText::FromString("Off"));
		bTempEnemyAlwaysRed = false;
	}
	else
	{
		EnemyStencilAlwaysRed_Control->OptionValueText->SetText(FText::FromString("On"));
		bTempEnemyAlwaysRed = true;
	}
}

void UWidget_GameOptions::OnUIOffscreenIndicatorsControlLeftButtonPressed()
{
	switch (TempUIIndicators)
	{
	case EIndicatorUISetting::ON:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::OFF;
			break;
		}
	case EIndicatorUISetting::ONSIDESONLY:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::ON;
			break;
		}
	case EIndicatorUISetting::OFF:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::ONSIDESONLY;
			break;
		}
	default:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::ON;
			break;
		}
	}

	SetUIOffscreenIndicatorsSetting();
}

void UWidget_GameOptions::OnUIOffscreenIndicatorsControlRightButtonPressed()
{
	switch (TempUIIndicators)
	{
	case EIndicatorUISetting::ON:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::ONSIDESONLY;
			break;
		}
	case EIndicatorUISetting::ONSIDESONLY:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::OFF;
			break;
		}
	case EIndicatorUISetting::OFF:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::ON;
			break;
		}
	default:
		{
			TempUIIndicators = (int32)EIndicatorUISetting::ON;
			break;
		}
	}

	SetUIOffscreenIndicatorsSetting();
}

void UWidget_GameOptions::SetUIOffscreenIndicatorsSetting()
{
	switch (TempUIIndicators)
	{
	case EIndicatorUISetting::ON:
		{
			UIOffscreenIndicators_Control->OptionValueText->SetText(FText::FromString("On"));
			break;
		}
	case EIndicatorUISetting::ONSIDESONLY:
		{
			UIOffscreenIndicators_Control->OptionValueText->SetText(FText::FromString("On (window edge only)"));
			break;
		}
	case EIndicatorUISetting::OFF:
		{
			UIOffscreenIndicators_Control->OptionValueText->SetText(FText::FromString("Off"));
			break;
		}
	default:
		{
			UIOffscreenIndicators_Control->OptionValueText->SetText(FText::FromString("On"));
			break;
		}
	}
}

void UWidget_GameOptions::OnUIOffscreenIndicatorSizeControlButtonPressed()
{
	//bool bLargeSize = bTempUIIndicatorSizeLarge;
	//bLargeSize = !bLargeSize;
	//bTempUIIndicatorSizeLarge = bLargeSize;


	if (bTempUIIndicatorSizeLarge == true)
		bTempUIIndicatorSizeLarge = false;
	else
		bTempUIIndicatorSizeLarge = true;

	SetUIOffscreenIndicatorSizeSettingText();
}

void UWidget_GameOptions::SetUIOffscreenIndicatorSizeSettingText()
{
	if (bTempUIIndicatorSizeLarge == true)
		UIOffscreenIndicatorSize_Control->OptionValueText->SetText(FText::FromString("Large"));
	else
		UIOffscreenIndicatorSize_Control->OptionValueText->SetText(FText::FromString("Small"));
}

void UWidget_GameOptions::OnMouseSensitivityScaleControlLeftButtonPressed()
{
	if (TempMouseSensitivityScale >= 1)
		TempMouseSensitivityScale -= 1;
	if (TempMouseSensitivityScale < 1)
		TempMouseSensitivityScale = 10;

	SetMouseSensitivityScaleSettingText();
}

void UWidget_GameOptions::OnMouseSensitivityScaleControlRightButtonPressed()
{
	if (TempMouseSensitivityScale < 11)
		TempMouseSensitivityScale += 1;
	if (TempMouseSensitivityScale >= 11)
		TempMouseSensitivityScale = 1;

	SetMouseSensitivityScaleSettingText();
}

void UWidget_GameOptions::SetMouseSensitivityScaleSettingText()
{
	MouseSensitivityScale_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(TempMouseSensitivityScale)));
}

void UWidget_GameOptions::OnCustomMouseCursorControlButtonPressed()
{
	if (TempCustomMouseCursor)
		TempCustomMouseCursor = false;
	else
		TempCustomMouseCursor = true;
	
	SetCustomMouseCursorSettingText();
}

void UWidget_GameOptions::SetCustomMouseCursorSettingText()
{
	if (TempCustomMouseCursor)
		CustomMouseCursor_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		CustomMouseCursor_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_GameOptions::OnControllerMenuSensitivityScaleControlLeftButtonPressed()
{
	if (TempControllerMenuSensitivityScale >= 1)
		TempControllerMenuSensitivityScale -= 1;
	if (TempControllerMenuSensitivityScale < 1)
		TempControllerMenuSensitivityScale = 10;

	SetControllerMenuSensitivityScaleSettingText();
}

void UWidget_GameOptions::OnControllerMenuSensitivityScaleControlRightButtonPressed()
{
	if (TempControllerMenuSensitivityScale < 11)
		TempControllerMenuSensitivityScale += 1;
	if (TempControllerMenuSensitivityScale >= 11)
		TempControllerMenuSensitivityScale = 1;

	SetControllerMenuSensitivityScaleSettingText();
}

void UWidget_GameOptions::SetControllerMenuSensitivityScaleSettingText()
{
	ControllerMenuSensitivityScale_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(TempControllerMenuSensitivityScale)));
}

void UWidget_GameOptions::SetQualityLevelText(UWidget_OptionSelector* _OptionSelectorWidget, int32 _QualityValue)
{
	switch (_QualityValue)
	{
	case 0:
		{
			_OptionSelectorWidget->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			_OptionSelectorWidget->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			_OptionSelectorWidget->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			_OptionSelectorWidget->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			_OptionSelectorWidget->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
}

