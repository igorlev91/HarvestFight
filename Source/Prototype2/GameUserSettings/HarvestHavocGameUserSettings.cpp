

#include "HarvestHavocGameUserSettings.h"


UHarvestHavocGameUserSettings::UHarvestHavocGameUserSettings(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	FieldOfView = 90;
	MasterGraphics = 3;
	MasterVolume = 10;
	MusicVolume = 10;
	AmbienceVolume = 10;
	SFXVolume = 10;
	bPlayerStencil = true;
	bEnemyAlwaysRed = false;
	UIIndicators = (int8)EIndicatorUISetting::ON;
	UIIndicatorSizeLarge = false;
	MouseSensitivityScale = 5;
	CustomMouseCursor = true;
	ControllerMenuSensitivityScale = 5;
}

UHarvestHavocGameUserSettings* UHarvestHavocGameUserSettings::GetHarvestHavocGameUserSettings()
{
	return Cast<UHarvestHavocGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UHarvestHavocGameUserSettings::SaveSettings()
{
	Super::SaveSettings();
	
	UE_LOG(LogTemp, Warning, TEXT("Saved game user settings"));
}

void UHarvestHavocGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	UE_LOG(LogTemp, Warning, TEXT("Loaded game user settings"));
}

void UHarvestHavocGameUserSettings::SetMasterVolume(int32 _Volume)
{
	MasterVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetMusicVolume(int32 _Volume)
{
	MusicVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetAmbienceVolume(int32 _Volume)
{
	AmbienceVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetSFXVolume(int32 _Volume)
{
	SFXVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetPlayerStencil(bool _bEnable)
{
	bPlayerStencil = _bEnable;
}

void UHarvestHavocGameUserSettings::SetUIIndicators(EIndicatorUISetting _Setting)
{
	UIIndicators = (int8)_Setting;
}

void UHarvestHavocGameUserSettings::SetUIIndicatorSizeLarge(bool _bLargeSize)
{
	UIIndicatorSizeLarge = _bLargeSize;
}
