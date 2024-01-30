

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
	VSync_Control->OptionText->SetText(FText::FromString("VSync"));
	HDRDisplay_Control->OptionText->SetText(FText::FromString("HDR Display"));
	FramerateLimit_Control->OptionText->SetText(FText::FromString("Framerate Limit"));
	TextureQuality_Control->OptionText->SetText(FText::FromString("Texture Quality"));
	FoliageQuality_Control->OptionText->SetText(FText::FromString("Foliage Quality"));
	ShadingQuality_Control->OptionText->SetText(FText::FromString("Shading Quality"));
	ShadowQuality_Control->OptionText->SetText(FText::FromString("Shadow Quality"));
	PostProcessingQuality_Control->OptionText->SetText(FText::FromString("Post Processing Quality"));
	AntiAliasingQuality_Control->OptionText->SetText(FText::FromString("Anti Aliasing Quality"));

	/* Set audio text & settings */
	MasterVolume_Control->OptionText->SetText(FText::FromString("Master Volume"));
	MusicVolume_Control->OptionText->SetText(FText::FromString("Music Volume"));
	AmbienceVolume_Control->OptionText->SetText(FText::FromString("Ambience Volume"));
	SFXVolume_Control->OptionText->SetText(FText::FromString("SFX Volume"));
	
	/* Set Game Option settings */
	PlayerStencil_Control->OptionText->SetText(FText::FromString("Player Coloured Outline"));
}

void UWidget_GameOptions::NativeConstruct()
{
	Super::NativeConstruct();

	/* Load settings */
	if (HHGameGameUserSettings)
	{
		HHGameGameUserSettings->LoadSettings(true);
		UE_LOG(LogTemp, Warning, TEXT("Loaded Master Volume: %d"), HHGameGameUserSettings->GetMasterVolume());
		UE_LOG(LogTemp, Warning, TEXT("Loaded Music Volume: %d"), HHGameGameUserSettings->GetMusicVolume());
		UE_LOG(LogTemp, Warning, TEXT("Loaded Ambience Volume: %d"), HHGameGameUserSettings->GetAmbienceVolume());
		UE_LOG(LogTemp, Warning, TEXT("Loaded SFX Volume: %d"), HHGameGameUserSettings->GetSFXVolume());

		UE_LOG(LogTemp, Warning, TEXT("Loaded Framerate: %f"), HHGameGameUserSettings->GetFrameRateLimit());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load settings"));
	}
	UpdateGameInstanceVariables();
	SetOptionsText();
	
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

	SetTextureQualitySetting();
	SetFoliageQualitySetting();
	SetShadingQualitySetting();
	SetShadowQualitySetting();
	SetPostProcessingQualitySetting();
	SetAntiAliasingQualitySetting();

	MasterVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMasterVolume())));
	MusicVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetMusicVolume())));
	AmbienceVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetAmbienceVolume())));
	SFXVolume_Control->OptionValueText->SetText(FText::FromString(FString::FromInt(HHGameGameUserSettings->GetSFXVolume())));

	/* VSync */
	if (HHGameGameUserSettings->GetPlayerStencil() == true)
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("On"));
	}
	else
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("Off"));
	}
	
}

void UWidget_GameOptions::UpdateGameInstanceVariables()
{
	if (!GameInstanceReference)
		return;

	/* Volume */
	GameInstanceReference->MusicVolume = (float)HHGameGameUserSettings->GetMusicVolume() * ((float)HHGameGameUserSettings->GetMasterVolume() / 10.0f) / 10.0f;
	GameInstanceReference->AmbienceVolume = (float)HHGameGameUserSettings->GetAmbienceVolume() * ((float)HHGameGameUserSettings->GetMasterVolume() / 10.0f) / 10.0f;
	GameInstanceReference->SFXVolume = (float)HHGameGameUserSettings->GetSFXVolume() * ((float)HHGameGameUserSettings->GetMasterVolume() / 10.0f) / 10.0f;

	/* Game Options */
	GameInstanceReference->bPlayerStentil = HHGameGameUserSettings->GetPlayerStencil();
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
	

	SetResolutionSetting();
}

void UWidget_GameOptions::OnResolutionControlRightButtonPressed()
{


	SetResolutionSetting();
}

void UWidget_GameOptions::SetResolutionSetting()
{
	//if (!HHGameGameUserSettings)
	//	return;
	//
	//switch (ResolutionOption)
	//{
	//case (uint8)EResolution::R_1280x720:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("1280x720"));
	//		ResolutionX = 1280;
	//		ResolutionY = 720;
	//		break;
	//	}
	//case (uint8)EResolution::R_1366x768:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("1366x768"));
	//		ResolutionX = 1366;
	//		ResolutionY = 768;
	//		break;
	//	}
	//case (uint8)EResolution::R_1600x900:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("1600x900"));
	//		ResolutionX = 1600;
	//		ResolutionY = 900;
	//		break;
	//	}
	//case (uint8)EResolution::R_1920x1080:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("1920x1080"));
	//		ResolutionX = 1920;
	//		ResolutionY = 1080;
	//		break;
	//	}
	//case (uint8)EResolution::R_2560x1440:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("2560x1440"));
	//		ResolutionX = 2560;
	//		ResolutionY = 1440;
	//		break;
	//	}
	//case (uint8)EResolution::R_2560x1080:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("2560x1080"));
	//		ResolutionX = 2560;
	//		ResolutionY = 1080;
	//		break;
	//	}
	//case (uint8)EResolution::R_3440x1440:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("3440x1440"));
	//		ResolutionX = 3440;
	//		ResolutionY = 1440;
	//		break;
	//	}
	//default:
	//	{
	//		Resolution_Control->OptionValueText->SetText(FText::FromString("Default 1280x720"));
	//		ResolutionX = 1280;
	//		ResolutionY = 720;
	//		break;
	//	}
	//}
	//
	//// Set the desired screen resolution (optional)
	//const FIntPoint Resolution = FIntPoint(ResolutionX, ResolutionY); // Change this to your desired resolution
	//HHGameGameUserSettings->SetScreenResolution(Resolution);
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

void UWidget_GameOptions::OnTextureQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetTextureQuality() >= 0)
		HHGameGameUserSettings->SetTextureQuality(HHGameGameUserSettings->GetTextureQuality() - 1);
	if (HHGameGameUserSettings->GetTextureQuality() < 0)
		HHGameGameUserSettings->SetTextureQuality(3);

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

	SetTextureQualitySetting();
}

void UWidget_GameOptions::SetTextureQualitySetting()
{
	switch (HHGameGameUserSettings->GetTextureQuality())
	{
	case 0:
		{
			TextureQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			TextureQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			TextureQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			TextureQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			TextureQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
}

void UWidget_GameOptions::OnFoliageQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetFoliageQuality() >= 0)
		HHGameGameUserSettings->SetFoliageQuality(HHGameGameUserSettings->GetFoliageQuality() - 1);
	if (HHGameGameUserSettings->GetFoliageQuality() < 0)
		HHGameGameUserSettings->SetFoliageQuality(3);

	SetFoliageQualitySetting();
}

void UWidget_GameOptions::OnFoliageQualityControlRightButtonPressed()
{
	if (HHGameGameUserSettings->GetFoliageQuality() < 4)
		HHGameGameUserSettings->SetFoliageQuality(HHGameGameUserSettings->GetFoliageQuality() + 1);
	if (HHGameGameUserSettings->GetFoliageQuality() >= 4)
		HHGameGameUserSettings->SetFoliageQuality(0);

	SetFoliageQualitySetting();
}

void UWidget_GameOptions::SetFoliageQualitySetting()
{
	switch (HHGameGameUserSettings->GetFoliageQuality())
	{
	case 0:
		{
			FoliageQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			FoliageQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			FoliageQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			FoliageQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			FoliageQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
}

void UWidget_GameOptions::OnShadingQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetShadingQuality() >= 0)
		HHGameGameUserSettings->SetShadingQuality(HHGameGameUserSettings->GetShadingQuality() - 1);
	if (HHGameGameUserSettings->GetShadingQuality() < 0)
		HHGameGameUserSettings->SetShadingQuality(3);


	SetShadingQualitySetting();
}

void UWidget_GameOptions::OnShadingQualityControlRightButtonPressed()
{
	if (HHGameGameUserSettings->GetShadingQuality() < 4)
		HHGameGameUserSettings->SetShadingQuality(HHGameGameUserSettings->GetShadingQuality() + 1);
	if (HHGameGameUserSettings->GetShadingQuality() >= 4)
		HHGameGameUserSettings->SetShadingQuality(0);

	SetShadingQualitySetting();
}

void UWidget_GameOptions::SetShadingQualitySetting()
{
	switch (HHGameGameUserSettings->GetShadingQuality())
	{
	case 0:
		{
			ShadingQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			ShadingQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			ShadingQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			ShadingQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			ShadingQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
}

void UWidget_GameOptions::OnShadowQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetShadowQuality() >= 0)
		HHGameGameUserSettings->SetShadowQuality(HHGameGameUserSettings->GetShadowQuality() - 1);
	if (HHGameGameUserSettings->GetShadowQuality() < 0)
		HHGameGameUserSettings->SetShadowQuality(3);

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

	SetShadowQualitySetting();
}

void UWidget_GameOptions::SetShadowQualitySetting()
{
	switch (HHGameGameUserSettings->GetShadowQuality())
	{
	case 0:
		{
			ShadowQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			ShadowQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			ShadowQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			ShadowQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			ShadowQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
}

void UWidget_GameOptions::OnPostProcessingQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetPostProcessingQuality() >= 0)
		HHGameGameUserSettings->SetPostProcessingQuality(HHGameGameUserSettings->GetPostProcessingQuality() - 1);
	if (HHGameGameUserSettings->GetPostProcessingQuality() < 0)
		HHGameGameUserSettings->SetPostProcessingQuality(3);

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

	SetPostProcessingQualitySetting();
}

void UWidget_GameOptions::SetPostProcessingQualitySetting()
{
	switch (HHGameGameUserSettings->GetPostProcessingQuality())
	{
	case 0:
		{
			PostProcessingQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			PostProcessingQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			PostProcessingQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			PostProcessingQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			PostProcessingQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
}

void UWidget_GameOptions::OnAntiAliasingQualityControlLeftButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetAntiAliasingQuality() >= 0)
		HHGameGameUserSettings->SetAntiAliasingQuality(HHGameGameUserSettings->GetAntiAliasingQuality() - 1);
	if (HHGameGameUserSettings->GetAntiAliasingQuality() < 0)
		HHGameGameUserSettings->SetAntiAliasingQuality(3);

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

	SetAntiAliasingQualitySetting();
}

void UWidget_GameOptions::SetAntiAliasingQualitySetting()
{
	switch (HHGameGameUserSettings->GetAntiAliasingQuality())
	{
	case 0:
		{
			AntiAliasingQuality_Control->OptionValueText->SetText(FText::FromString("Potato"));
			break;
		}
	case 1:
		{
			AntiAliasingQuality_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case 2:
		{
			AntiAliasingQuality_Control->OptionValueText->SetText(FText::FromString("High"));
			break;
		}
	case 3:
		{
			AntiAliasingQuality_Control->OptionValueText->SetText(FText::FromString("Epic"));
			break;
		}
	default:
		{
			AntiAliasingQuality_Control->OptionValueText->SetText(FText::FromString("Default Medium"));
		}
	}
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
}

void UWidget_GameOptions::OnPlayerStencilControlButtonPressed()
{
	if (!HHGameGameUserSettings)
		return;
	
	if (HHGameGameUserSettings->GetPlayerStencil() == true)
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("Off"));
		HHGameGameUserSettings->SetPlayerStencil(false);
	}
	else
	{
		PlayerStencil_Control->OptionValueText->SetText(FText::FromString("On"));
		HHGameGameUserSettings->SetPlayerStencil(true);
	}
}

